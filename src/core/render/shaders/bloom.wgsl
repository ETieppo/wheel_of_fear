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
@group(0) @binding(1) var lit_scene: texture_2d<f32>;
@group(0) @binding(2) var lit_sampler: sampler;

const bloom_threshold: f32 = 0.55;

fn brightPart(coordinate: vec2<f32>) -> vec3<f32> {
    let color = textureSample(lit_scene, lit_sampler, coordinate).rgb;
    let brightness = max(max(color.r, color.g), color.b);
    let knee = clamp((brightness - bloom_threshold) / max(brightness, 0.0001), 0.0, 1.0);

    return color * knee;
}

@fragment fn fs(input: VertexOutputProps) -> @location(0) vec4<f32> {
    let bloom_size = post.target_info.xy / f32(4);
    let texel = 1.0 / bloom_size;
    let coordinate = input.position.xy / bloom_size;
    var sum = brightPart(coordinate) * 0.16;
    var total = 0.16;

    for (var ring = 1; ring <= 3; ring = ring + 1) {
        let radius = f32(ring) * 1.6;
        let weight = 0.14 / f32(ring);

        for (var spoke = 0; spoke < 8; spoke = spoke + 1) {
            let angle = f32(spoke) * 0.785398 + f32(ring) * 0.39;
            let offset = vec2<f32>(cos(angle), sin(angle)) * radius * texel;

            sum = sum + brightPart(coordinate + offset) * weight;
            total = total + weight;
        }
    }

    return vec4<f32>(sum / total, 1.0);
}
