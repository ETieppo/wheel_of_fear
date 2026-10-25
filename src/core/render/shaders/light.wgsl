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

fn isWallAt(point: vec2<f32>) -> bool {
    return wallAt(point) > 0.5;
}

fn blocksLightAt(point: vec2<f32>) -> bool {
    return wallAt(point) > 0.25;
}

fn isSameTile(first: vec2<f32>, second: vec2<f32>) -> bool {
    return first.x == second.x && first.y == second.y;
}

fn rayVisibility(origin: vec2<f32>, destination: vec2<f32>, own_tile: vec2<f32>, steps: f32, includes_shadow_casters: bool) -> f32 {
    let delta = destination - origin;
    let destination_is_wall = isWallAt(destination);
    let step_count = i32(steps);
    var has_entered_wall = false;

    for (var step_index = 1; step_index < step_count; step_index = step_index + 1) {
        let sample_point = origin + delta * (f32(step_index) / steps);

        if (isSameTile(floor(sample_point / post.map_info.z), own_tile)) {
            return 1.0;
        }

        let is_wall_sample = isWallAt(sample_point);

        if (includes_shadow_casters && !is_wall_sample && blocksLightAt(sample_point)) {
            return 0.0;
        }

        if (is_wall_sample && !destination_is_wall) {
            return 0.0;
        }

        if (is_wall_sample) {
            has_entered_wall = true;
        } else if (has_entered_wall) {
            return 0.0;
        }
    }

    return 1.0;
}

fn viewerVisibility(world: vec2<f32>, own_tile: vec2<f32>) -> f32 {
    if (post.viewer.z < 0.5) {
        return 1.0;
    }

    let steps = clamp(length(world - post.viewer.xy) / 3.0, 1.0, 160.0);

    return rayVisibility(post.viewer.xy, world, own_tile, steps, false);
}

@fragment fn fs(input: VertexOutputProps) -> LightOutputProps {
    let coordinate = input.position.xy / post.target_info.zw;
    let world = post.view_origin + coordinate * post.view_size;
    let own_tile = floor(world / post.map_info.z);

    var point_light = vec3<f32>(0.0, 0.0, 0.0);
    var spot_light = vec3<f32>(0.0, 0.0, 0.0);
    var lit_area_level = 0.0;
    var revealed_level = 0.0;
    var point_direction_sum = vec2<f32>(0.0, 0.0);
    var point_weight_sum = 0.0;
    var spot_direction_sum = vec2<f32>(0.0, 0.0);
    var spot_weight_sum = 0.0;
    let count = i32(post.light_count);

    for (var index = 0; index < count; index = index + 1) {
        let source = post.lights[index];
        let source_position = source.position_radius.xy;
        let radius = source.position_radius.z;
        let delta = world - source_position;
        let distance_to_source = length(delta);
        let falloff = clamp(1.0 - distance_to_source / radius, 0.0, 1.0);
        var attenuation = falloff * falloff * (1.0 / (1.0 + distance_to_source * distance_to_source * 0.00015));

        let is_spotlight = source.direction_cone.z > -1.5;

        if (is_spotlight) {
            let direction = normalize(delta + vec2<f32>(0.0001, 0.0));
            let alignment = dot(direction, source.direction_cone.xy);
            let cone = smoothstep(source.direction_cone.z, source.direction_cone.w, alignment);
            let near_halo = clamp(1.0 - distance_to_source / 12.0, 0.0, 1.0);
            attenuation = attenuation * max(cone, near_halo * 0.45);
        }

        let has_bounds = source.bounds.z > source.bounds.x;

        if (has_bounds) {
            let is_inside_bounds = all(world >= source.bounds.xy) && all(world <= source.bounds.zw);
            attenuation = attenuation * select(0.0, 1.0, is_inside_bounds);
        } else if (attenuation > 0.001 && source.color.a > 0.5) {
            let steps = clamp(distance_to_source / 4.0, 2.0, 80.0);
            attenuation = attenuation * rayVisibility(source_position, world, own_tile, steps, is_spotlight);
        }

        let flicker_noise = hash2(vec2<f32>(floor(post.time * 20.0), source_position.x));
        let flicker = 1.0 - source.animation.x * (0.5 + 0.5 * sin(post.time * (13.0 + source.animation.y) + hash2(source_position) * 6.0)) * flicker_noise;
        let contribution = source.color.rgb * source.position_radius.w * attenuation * flicker;

        let weight = dot(contribution, vec3<f32>(0.3, 0.59, 0.11));
        var direction_toward_source = vec2<f32>(0.0, 0.0);

        if (distance_to_source > 0.5) {
            direction_toward_source = -normalize(delta) * weight;
        }

        if (is_spotlight) {
            spot_light = spot_light + contribution;
            spot_direction_sum = spot_direction_sum + direction_toward_source;
            spot_weight_sum = spot_weight_sum + weight;
        } else {
            point_light = point_light + contribution;
            point_direction_sum = point_direction_sum + direction_toward_source;
            point_weight_sum = point_weight_sum + weight;
        }

        if (!is_spotlight && source.color.a > 0.5) {
            lit_area_level = lit_area_level + weight;
        }

        if (source.animation.z > 0.5) {
            revealed_level = revealed_level + weight;
        }
    }

    let spot_fade = 1.0 - smoothstep(0.05, 0.4, lit_area_level);
    let light = point_light + spot_light * spot_fade;
    let direction_sum = point_direction_sum + spot_direction_sum * spot_fade;
    let weight_sum = point_weight_sum + spot_weight_sum * spot_fade;

    var output: LightOutputProps;
    let revealed = smoothstep(0.02, 0.12, revealed_level);
    output.irradiance = vec4<f32>(light, max(viewerVisibility(world, own_tile), revealed));
    output.direction = vec4<f32>(direction_sum, weight_sum, 1.0);

    return output;
}
