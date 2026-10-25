struct GpuLightProps {
    position_radius: vec4<f32>,
    direction_cone: vec4<f32>,
    color: vec4<f32>,
    animation: vec4<f32>,
    bounds: vec4<f32>,
};

struct PostProcessProps {
    view_origin: vec2<f32>,
    view_size: vec2<f32>,
    screen_size: vec2<f32>,
    time: f32,
    light_count: f32,
    ambient: vec4<f32>,
    effects: vec4<f32>,
    tint: vec4<f32>,
    map_info: vec4<f32>,
    target_info: vec4<f32>,
    viewer: vec4<f32>,
    lights: array<GpuLightProps, 32>,
};

struct VertexOutputProps {
    @builtin(position) position: vec4<f32>,
};

struct SurfaceProps {
    albedo: vec3<f32>,
    normal: vec3<f32>,
    occlusion: f32,
    shininess: f32,
};

@vertex fn vs(@builtin(vertex_index) vertex_index: u32) -> VertexOutputProps {
    var points = array<vec2<f32>, 3>(
        vec2<f32>(-1.0, -1.0),
        vec2<f32>(3.0, -1.0),
        vec2<f32>(-1.0, 3.0)
    );
    var output: VertexOutputProps;
    output.position = vec4<f32>(points[vertex_index], 0.0, 1.0);
    return output;
}

@group(0) @binding(0) var<uniform> post: PostProcessProps;
@group(0) @binding(1) var scene: texture_2d<f32>;
@group(0) @binding(2) var glow: texture_2d<f32>;
@group(0) @binding(3) var irradiance: texture_2d<f32>;
@group(0) @binding(4) var light_direction: texture_2d<f32>;
@group(0) @binding(5) var wall_map: texture_2d<f32>;
@group(0) @binding(6) var clamp_sampler: sampler;
@group(0) @binding(7) var repeat_sampler: sampler;
@group(0) @binding(8) var floor_color: texture_2d<f32>;
@group(0) @binding(9) var floor_normal: texture_2d<f32>;
@group(0) @binding(10) var wall_color: texture_2d<f32>;
@group(0) @binding(11) var wall_normal: texture_2d<f32>;
@group(0) @binding(12) var face_color: texture_2d<f32>;
@group(0) @binding(13) var face_normal: texture_2d<f32>;

const floor_repeat_units: f32 = 64.0;
const wall_repeat_units: f32 = 48.0;
const face_height: f32 = 0.55;

fn hash2(point: vec2<f32>) -> f32 {
    return fract(sin(dot(point, vec2<f32>(127.1, 311.7))) * 43758.5453);
}

fn valueNoise(point: vec2<f32>) -> f32 {
    let cell = floor(point);
    let local = fract(point);
    let smooth_local = local * local * (3.0 - 2.0 * local);
    let a = hash2(cell);
    let b = hash2(cell + vec2<f32>(1.0, 0.0));
    let c = hash2(cell + vec2<f32>(0.0, 1.0));
    let d = hash2(cell + vec2<f32>(1.0, 1.0));

    return mix(mix(a, b, smooth_local.x), mix(c, d, smooth_local.x), smooth_local.y);
}

fn isInsideMap(tile: vec2<f32>) -> bool {
    return tile.x >= 0.0 && tile.y >= 0.0 && tile.x < post.map_info.x && tile.y < post.map_info.y;
}

fn wallAtTile(tile: vec2<f32>) -> f32 {
    if (!isInsideMap(tile)) {
        return 1.0;
    }

    return step(0.5, textureLoad(wall_map, vec2<i32>(i32(tile.x), i32(tile.y)), 0).r);
}

fn decodeNormal(sample: vec4<f32>) -> vec3<f32> {
    let unpacked = sample.xyz * 2.0 - vec3<f32>(1.0, 1.0, 1.0);
    return normalize(vec3<f32>(unpacked.x, -unpacked.y, max(unpacked.z, 0.2)));
}

fn nonNegativeLog2(value: f32) -> f32 {
    var level = 0.0;
    var octave = 1.0;

    for (var index = 0; index < 16; index = index + 1) {
        level = level + clamp(value / octave - 1.0, 0.0, 1.0);
        octave = octave * 2.0;
    }

    return level;
}

fn materialLevel(world_per_pixel: f32, repeat_units: f32) -> f32 {
    return nonNegativeLog2(world_per_pixel * 1024.0 / repeat_units);
}

fn floorOcclusion(tile: vec2<f32>, local: vec2<f32>) -> f32 {
    var occlusion = 1.0;
    let reach = 0.6;

    occlusion = occlusion * mix(1.0, smoothstep(0.0, reach, local.x), wallAtTile(tile + vec2<f32>(-1.0, 0.0)));
    occlusion = occlusion * mix(1.0, smoothstep(0.0, reach, 1.0 - local.x), wallAtTile(tile + vec2<f32>(1.0, 0.0)));
    occlusion = occlusion * mix(1.0, smoothstep(0.0, reach * 1.4, local.y), wallAtTile(tile + vec2<f32>(0.0, -1.0)));
    occlusion = occlusion * mix(1.0, smoothstep(0.0, reach * 0.6, 1.0 - local.y), wallAtTile(tile + vec2<f32>(0.0, 1.0)));

    let corner = min(length(local), reach) / reach;
    occlusion = occlusion * mix(1.0, corner, wallAtTile(tile + vec2<f32>(-1.0, -1.0)) * (1.0 - wallAtTile(tile + vec2<f32>(-1.0, 0.0))) * (1.0 - wallAtTile(tile + vec2<f32>(0.0, -1.0))));

    return 0.35 + 0.65 * occlusion;
}

fn groundSurface(world: vec2<f32>, world_per_pixel: f32) -> SurfaceProps {
    let tile = floor(world / post.map_info.z);
    let local = fract(world / post.map_info.z);
    var surface: SurfaceProps;

    if (!isInsideMap(tile)) {
        surface.albedo = vec3<f32>(0.0, 0.0, 0.0);
        surface.normal = vec3<f32>(0.0, 0.0, 1.0);
        surface.occlusion = 0.0;
        surface.shininess = 0.0;
        return surface;
    }

    if (wallAtTile(tile) > 0.5) {
        let shows_face = wallAtTile(tile + vec2<f32>(0.0, 1.0)) < 0.5;
        if (shows_face && local.y > 1.0 - face_height) {
            let face_v = (local.y - (1.0 - face_height)) / face_height;
            let face_uv = vec2<f32>(world.x / 32.0, face_v * 0.45);
            let color = textureSampleLevel(face_color, repeat_sampler, face_uv, materialLevel(world_per_pixel, 32.0)).rgb;
            let bumped = decodeNormal(textureSampleLevel(face_normal, repeat_sampler, face_uv, materialLevel(world_per_pixel, 32.0)));
            surface.albedo = color * (0.62 - 0.35 * face_v) * vec3<f32>(0.85, 0.9, 1.0);
            surface.normal = normalize(vec3<f32>(bumped.x * 0.6, 0.85, 0.55));
            surface.occlusion = 0.55 + 0.45 * (1.0 - face_v);
            surface.shininess = 0.5;
            return surface;
        }

        let wall_uv = world / wall_repeat_units;
        let color = textureSampleLevel(wall_color, repeat_sampler, wall_uv, materialLevel(world_per_pixel, wall_repeat_units)).rgb;
        let bumped = decodeNormal(textureSampleLevel(wall_normal, repeat_sampler, wall_uv, materialLevel(world_per_pixel, wall_repeat_units)));
        var edge = 1.0;

        if (wallAtTile(tile + vec2<f32>(-1.0, 0.0)) < 0.5) { edge = min(edge, local.x); }
        if (wallAtTile(tile + vec2<f32>(1.0, 0.0)) < 0.5) { edge = min(edge, 1.0 - local.x); }
        if (wallAtTile(tile + vec2<f32>(0.0, -1.0)) < 0.5) { edge = min(edge, local.y); }
        if (shows_face) { edge = min(edge, (1.0 - face_height) - local.y); }

        let rim = 1.0 - smoothstep(0.0, 0.12, edge);
        let rim_line = 1.0 - smoothstep(0.0, 0.035, edge);

        surface.albedo = color * 0.3 + vec3<f32>(0.25, 0.28, 0.34) * rim + vec3<f32>(0.35, 0.4, 0.48) * rim_line;
        surface.normal = bumped;
        surface.occlusion = 0.8;
        surface.shininess = 0.35;

        return surface;
    }

    let floor_uv = world / floor_repeat_units;
    let color = textureSampleLevel(floor_color, repeat_sampler, floor_uv, materialLevel(world_per_pixel, floor_repeat_units)).rgb;
    let bumped = decodeNormal(textureSampleLevel(floor_normal, repeat_sampler, floor_uv, materialLevel(world_per_pixel, floor_repeat_units)));
    let grime = 0.8 + 0.35 * valueNoise(world / 40.0) - 0.15 * valueNoise(world / 9.0);
    surface.albedo = color * grime;
    surface.normal = bumped;
    surface.occlusion = floorOcclusion(tile, local);
    surface.shininess = 1.0;

    return surface;
}

@fragment fn fs(input: VertexOutputProps) -> @location(0) vec4<f32> {
    let pixel = vec2<i32>(i32(input.position.x), i32(input.position.y));
    let coordinate = input.position.xy / post.target_info.xy;
    let world = post.view_origin + coordinate * post.view_size;
    let world_per_pixel = post.view_size.x / post.target_info.x;
    let ground = groundSurface(world, world_per_pixel);

    let sprite = textureLoad(scene, pixel, 0);
    let emissive = textureLoad(glow, pixel, 0);
    let albedo = sprite.rgb + ground.albedo * (1.0 - sprite.a);
    let normal = normalize(mix(ground.normal, vec3<f32>(0.0, 0.0, 1.0), sprite.a));
    let occlusion = mix(ground.occlusion, 1.0, sprite.a);
    let shininess = ground.shininess * (1.0 - sprite.a);

    let irradiance_sample = textureSample(irradiance, clamp_sampler, coordinate);
    let light = irradiance_sample.rgb;
    let line_of_sight = smoothstep(0.15, 0.85, irradiance_sample.a);
    let direction_data = textureSample(light_direction, clamp_sampler, coordinate);
    let planar = direction_data.xy / max(direction_data.z, 0.0001);
    let toward_light = normalize(vec3<f32>(planar * 0.85, 0.55));
    let lambert = max(dot(normal, toward_light), 0.0) / max(toward_light.z, 0.25);
    let diffuse = light * clamp(lambert, 0.0, 2.2);
    let half_vector = normalize(toward_light + vec3<f32>(0.0, 0.0, 1.0));
    let specular = light * pow(max(dot(normal, half_vector), 0.0), 36.0) * 0.55 * shininess;

    let ambient = post.ambient.rgb * occlusion * (0.75 + 0.5 * valueNoise(world / 70.0 + vec2<f32>(post.time * 0.02, 0.0)));
    var rgb = albedo * (ambient + diffuse * occlusion) + specular * occlusion + emissive.rgb * 1.6;
    let haze = valueNoise(world / 22.0 + vec2<f32>(post.time * 0.15, post.time * 0.07));
    rgb = rgb + vec3<f32>(0.006, 0.008, 0.014) * haze;
    rgb = rgb * mix(0.02, 1.0, line_of_sight);

    return vec4<f32>(rgb, 1.0);
}
