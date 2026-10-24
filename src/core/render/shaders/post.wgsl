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

fn hash2(point: vec2<f32>) -> f32 {
    return fract(sin(dot(point, vec2<f32>(127.1, 311.7))) * 43758.5453);
}

@group(0) @binding(0) var<uniform> post: PostProcessProps;
@group(0) @binding(1) var lit_scene: texture_2d<f32>;
@group(0) @binding(2) var bloom: texture_2d<f32>;
@group(0) @binding(3) var lit_sampler: sampler;

fn bloomAt(coordinate: vec2<f32>) -> vec3<f32> {
    let texel = 1.0 / (post.target_info.xy / 4.0);
    var sum = textureSample(bloom, lit_sampler, coordinate).rgb * 0.4;

    sum = sum + textureSample(bloom, lit_sampler, coordinate + vec2<f32>(texel.x * 3.0, 0.0)).rgb * 0.15;
    sum = sum + textureSample(bloom, lit_sampler, coordinate - vec2<f32>(texel.x * 3.0, 0.0)).rgb * 0.15;
    sum = sum + textureSample(bloom, lit_sampler, coordinate + vec2<f32>(0.0, texel.y * 3.0)).rgb * 0.15;
    sum = sum + textureSample(bloom, lit_sampler, coordinate - vec2<f32>(0.0, texel.y * 3.0)).rgb * 0.15;

    return sum;
}

fn toneMap(color: vec3<f32>) -> vec3<f32> {
    let a = 2.51;
    let b = 0.03;
    let c = 2.43;
    let d = 0.59;
    let e = 0.14;
    let mapped = clamp((color * (a * color + b)) / (color * (c * color + d) + e), vec3<f32>(0.0), vec3<f32>(1.0));

    return pow(mapped, vec3<f32>(0.82));
}

@fragment fn fs(input: VertexOutputProps) -> @location(0) vec4<f32> {
    let scale = min(post.screen_size.x / post.target_info.x, post.screen_size.y / post.target_info.y);
    let offset = (post.screen_size - post.target_info.xy * scale) * 0.5;
    let coordinate = (input.position.xy - offset) / (post.target_info.xy * scale);
    let centered = coordinate - vec2<f32>(0.5, 0.5);

    let danger = post.effects.x;
    let hurt = post.effects.y;
    let aberration = (0.0008 + 0.004 * danger + 0.01 * hurt) * length(centered) * 2.0;
    let wobble = sin(coordinate.y * 30.0 + post.time * 5.0) * 0.0012 * danger;
    let warped = coordinate + vec2<f32>(wobble, 0.0);
    let red = textureSample(lit_scene, lit_sampler, warped + centered * aberration).r;
    let green = textureSample(lit_scene, lit_sampler, warped).g;
    let blue = textureSample(lit_scene, lit_sampler, warped - centered * aberration).b;
    var rgb = vec3<f32>(red, green, blue) + bloomAt(warped) * 1.1;

    let luminance = dot(rgb, vec3<f32>(0.299, 0.587, 0.114));
    rgb = mix(rgb, vec3<f32>(luminance), post.tint.z);
    rgb = mix(rgb, rgb * vec3<f32>(0.85, 0.7, 1.3), post.tint.x * 0.35);
    rgb = rgb + vec3<f32>(0.5, 0.02, 0.03) * hurt * (0.35 + 0.65 * length(centered));

    rgb = toneMap(rgb * 1.15);
    let vignette = 1.0 - dot(centered, centered) * (0.95 + 0.7 * danger);

    rgb = rgb * clamp(vignette, 0.0, 1.0);
    let grain = hash2(input.position.xy + vec2<f32>(fract(post.time * 7.31) * 517.0, fract(post.time * 3.17) * 911.0)) - 0.5;

    rgb = rgb + grain * (0.018 + 0.03 * danger);
    rgb = rgb * (1.0 - post.effects.z * 0.85);
    rgb = rgb * (1.0 - post.effects.w);

    if (coordinate.x < 0.0 || coordinate.y < 0.0 || coordinate.x > 1.0 || coordinate.y > 1.0) {
        rgb = vec3<f32>(0.0, 0.0, 0.0);
    }

    return vec4<f32>(rgb, 1.0);
}
