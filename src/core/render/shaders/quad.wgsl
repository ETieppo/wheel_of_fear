struct Quad {
    position_size: vec4<f32>,
    texture_region: vec4<f32>,
    color: vec4<f32>,
    parameters: vec4<f32>,
};

struct View {
    origin: vec2<f32>,
    size: vec2<f32>,
    atlas_size: vec2<f32>,
    time: f32,
    padding: f32,
};

@group(0) @binding(0) var<uniform> view: View;
@group(0) @binding(1) var<storage, read> quads: array<Quad>;
@group(0) @binding(2) var atlas_sampler: sampler;
@group(0) @binding(3) var atlas: texture_2d<f32>;

struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) texture_coordinate: vec2<f32>,
    @location(1) local: vec2<f32>,
    @location(2) color: vec4<f32>,
    @location(3) parameters: vec4<f32>,
    @location(4) world: vec2<f32>,
};

@vertex fn vs(@builtin(vertex_index) vertex_index: u32) -> VertexOutput {
    let quad = quads[vertex_index / 6u];
    var corners = array<vec2<f32>, 6>(
        vec2<f32>(-0.5, -0.5),
        vec2<f32>(0.5, -0.5),
        vec2<f32>(-0.5, 0.5),
        vec2<f32>(-0.5, 0.5),
        vec2<f32>(0.5, -0.5),
        vec2<f32>(0.5, 0.5)
    );
    let corner = corners[vertex_index % 6u];
    let rotation = quad.parameters.x;
    let cosine = cos(rotation);
    let sine = sin(rotation);
    let scaled = corner * quad.position_size.zw;
    let rotated = vec2<f32>(scaled.x * cosine - scaled.y * sine, scaled.x * sine + scaled.y * cosine) + quad.position_size.xy;
    let normalized_device = (rotated - view.origin) / view.size * 2.0 - vec2<f32>(1.0, 1.0);
    var output: VertexOutput;
    output.position = vec4<f32>(normalized_device.x, -normalized_device.y, 0.0, 1.0);
    output.texture_coordinate = quad.texture_region.xy + (quad.texture_region.zw - quad.texture_region.xy) * (corner + vec2<f32>(0.5, 0.5));
    output.local = corner * 2.0;
    output.color = quad.color;
    output.parameters = quad.parameters;
    output.world = rotated;
    return output;
}

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

fn premultiplied(color: vec4<f32>) -> vec4<f32> {
    return vec4<f32>(color.rgb * color.a, color.a);
}

@fragment fn fs(input: VertexOutput) -> @location(0) vec4<f32> {
    let texel = textureSample(atlas, atlas_sampler, input.texture_coordinate);
    let kind = input.parameters.y;
    let radial = length(input.local);
    let radial_width = fwidth(radial);
    var result = premultiplied(input.color);

    if (kind < 0.5) {
        var rgb = texel.rgb / max(texel.a, 0.001) * input.color.rgb;
        let petrify = input.parameters.z;
        if (petrify > 0.0) {
            let luminance = dot(rgb, vec3<f32>(0.299, 0.587, 0.114));
            let grain = valueNoise(input.world * 1.5) * 0.6 + valueNoise(input.world * 5.0) * 0.4;
            let plaster = vec3<f32>(0.80, 0.78, 0.74) * (0.55 + 0.6 * luminance) * (0.85 + 0.3 * grain);
            rgb = mix(rgb, plaster, clamp(petrify, 0.0, 1.0));
            let crack_field = abs(valueNoise(input.world * 0.9 + vec2<f32>(3.0, 7.0)) - 0.5);
            let crack = 1.0 - smoothstep(0.0, 0.035 + 0.04 * clamp(petrify - 1.0, 0.0, 1.0), crack_field);
            rgb = rgb * (1.0 - 0.65 * crack * clamp(petrify - 0.6, 0.0, 1.0));
        }
        let alpha = texel.a * input.color.a;
        result = vec4<f32>(rgb * alpha, alpha);
    } else if (kind < 1.5) {
        result = premultiplied(input.color);
    } else if (kind < 2.5) {
        let softness = max(input.parameters.z, 0.01);
        let disc_alpha = pow(clamp(1.0 - radial, 0.0, 1.0), softness);
        result = premultiplied(vec4<f32>(input.color.rgb, input.color.a * disc_alpha));
    } else if (kind < 3.5) {
        let thickness = max(input.parameters.z, 0.02);
        let ring_distance = abs(radial - (1.0 - thickness));
        let edge = radial_width * 1.5;
        let ring_alpha = 1.0 - smoothstep(thickness - edge, thickness, ring_distance);
        result = premultiplied(vec4<f32>(input.color.rgb, input.color.a * ring_alpha));
    } else if (kind < 4.5) {
        result = premultiplied(vec4<f32>(input.color.rgb, input.color.a * texel.a));
    } else if (kind < 5.5) {
        let row = input.position.y;
        let scanline = 0.5 + 0.5 * sin(row * 0.9 + view.time * 4.0);
        let roll = fract(view.time * 0.15);
        let band = 1.0 - clamp(abs(fract(row / view.size.y - roll) - 0.5) * 12.0, 0.0, 1.0);
        let static_noise = valueNoise(input.position.xy * 0.35 + vec2<f32>(view.time * 40.0, 0.0));
        let vignette = clamp(1.0 - radial * radial * 6.0, 0.25, 1.0);
        let glow = (0.06 + 0.04 * scanline + 0.035 * band + 0.05 * static_noise) * vignette;
        result = premultiplied(vec4<f32>(input.color.rgb * glow * 3.0, input.color.a * (0.94 - 0.25 * (1.0 - vignette))));
    } else {
        let fade = clamp(1.0 - (input.local.x * 0.5 + 0.5), 0.0, 1.0);
        let edge = clamp(1.0 - abs(input.local.y), 0.0, 1.0);
        let dust = 0.7 + 0.3 * valueNoise(input.world * 0.8 + vec2<f32>(view.time * 3.0, view.time * 1.3));
        result = premultiplied(vec4<f32>(input.color.rgb, input.color.a * fade * edge * edge * dust));
    }
    return result;
}
