struct GpuLightProps {
    position_radius: vec4<f32>,
    direction_cone: vec4<f32>,
    color: vec4<f32>,
    animation: vec4<f32>,
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

struct LightOutputProps {
    @location(0) irradiance: vec4<f32>,
    @location(1) direction: vec4<f32>,
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

fn hash2(point: vec2<f32>) -> f32 {
    return fract(sin(dot(point, vec2<f32>(127.1, 311.7))) * 43758.5453);
}

@group(0) @binding(0) var<uniform> post: PostProcessProps;
@group(0) @binding(1) var wall_map: texture_2d<f32>;

fn wallAt(point: vec2<f32>) -> f32 {
    let tile = floor(point / post.map_info.z);

    if (tile.x < 0.0 || tile.y < 0.0 || tile.x >= post.map_info.x || tile.y >= post.map_info.y) {
        return 1.0;
    }

    return textureLoad(wall_map, vec2<i32>(i32(tile.x), i32(tile.y)), 0).r;
}

fn viewerVisibility(world: vec2<f32>, own_tile: vec2<f32>) -> f32 {
    if (post.viewer.z < 0.5) {
        return 1.0;
    }

    let origin = post.viewer.xy;
    let delta = world - origin;
    let distance_to_viewer = length(delta);
    let steps = clamp(distance_to_viewer / 3.0, 1.0, 160.0);
    let step_count = i32(steps);
    var visibility = 1.0;
    var finished = false;

    for (var step_index = 1; step_index < step_count && !finished; step_index = step_index + 1) {
        let sample_point = origin + delta * (f32(step_index) / steps);
        let sample_tile = floor(sample_point / post.map_info.z);

        if (sample_tile.x == own_tile.x && sample_tile.y == own_tile.y) {
            finished = true;
        } else if (wallAt(sample_point) > 0.5) {
            visibility = 0.0;
            finished = true;
        }
    }

    return visibility;
}

@fragment fn fs(input: VertexOutputProps) -> LightOutputProps {
    let coordinate = input.position.xy / post.target_info.zw;
    let world = post.view_origin + coordinate * post.view_size;
    let own_tile = floor(world / post.map_info.z);

    var light = vec3<f32>(0.0, 0.0, 0.0);
    var direction_sum = vec2<f32>(0.0, 0.0);
    var weight_sum = 0.0;
    let count = i32(post.light_count);

    for (var index = 0; index < count; index = index + 1) {
        let source = post.lights[index];
        let source_position = source.position_radius.xy;
        let radius = source.position_radius.z;
        let delta = world - source_position;
        let distance_to_source = length(delta);
        let falloff = clamp(1.0 - distance_to_source / radius, 0.0, 1.0);
        var attenuation = falloff * falloff * (1.0 / (1.0 + distance_to_source * distance_to_source * 0.00015));

        if (source.direction_cone.z > -1.5) {
            let direction = normalize(delta + vec2<f32>(0.0001, 0.0));
            let alignment = dot(direction, source.direction_cone.xy);
            let cone = smoothstep(source.direction_cone.z, source.direction_cone.w, alignment);
            let near_halo = clamp(1.0 - distance_to_source / 12.0, 0.0, 1.0);
            attenuation = attenuation * max(cone, near_halo * 0.45);
        }

        if (attenuation > 0.001 && source.color.a > 0.5) {
            let steps = clamp(distance_to_source / 4.0, 2.0, 80.0);
            let step_count = i32(steps);
            var visibility = 1.0;
            var finished = false;

            for (var step_index = 1; step_index < step_count && !finished; step_index = step_index + 1) {
                let sample_point = source_position + delta * (f32(step_index) / steps);
                let sample_tile = floor(sample_point / post.map_info.z);

                if (sample_tile.x == own_tile.x && sample_tile.y == own_tile.y) {
                    finished = true;
                } else if (wallAt(sample_point) > 0.5) {
                    visibility = 0.0;
                    finished = true;
                }
            }

            attenuation = attenuation * visibility;
        }

        let flicker_noise = hash2(vec2<f32>(floor(post.time * 20.0), source_position.x));
        let flicker = 1.0 - source.animation.x * (0.5 + 0.5 * sin(post.time * (13.0 + source.animation.y) + hash2(source_position) * 6.0)) * flicker_noise;
        let contribution = source.color.rgb * source.position_radius.w * attenuation * flicker;

        light = light + contribution;
        let weight = dot(contribution, vec3<f32>(0.3, 0.59, 0.11));

        if (distance_to_source > 0.5) {
            direction_sum = direction_sum - normalize(delta) * weight;
        }

        weight_sum = weight_sum + weight;
    }

    var output: LightOutputProps;
    output.irradiance = vec4<f32>(light, viewerVisibility(world, own_tile));
    output.direction = vec4<f32>(direction_sum, weight_sum, 1.0);

    return output;
}
