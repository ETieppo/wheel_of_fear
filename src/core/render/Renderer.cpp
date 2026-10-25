#include "core/render/Renderer.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/Utf8Decoder.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace wheel_of_fear {
  namespace {
    constexpr float blank_texel
      = 1.5F / static_cast<float>(RenderConfig::atlas_size);
    constexpr std::array<float, 4>
      blank_region {blank_texel, blank_texel, blank_texel, blank_texel};

    constexpr std::uint8_t wall_texel          = 255;
    constexpr std::uint8_t shadow_caster_texel = 96;

    std::uint8_t toByte(float channel) {
      return static_cast<std::uint8_t>(
        std::lround(std::clamp(channel, 0.0F, 1.0F) * 255.0F)
      );
    }
  } // namespace

  Renderer::Renderer() {
    typefaces_.bakeInto(atlas_);
  }

  std::optional<GroundMaterialsDto> Renderer::takeGroundMaterials() {
    std::optional<GroundMaterialsDto> materials
      = std::move(pending_ground_materials_);

    pending_ground_materials_.reset();
    return materials;
  }

  void Renderer::beginFrame(
    Vec2Props camera,
    float     time
  ) {
    frame_.reset();
    frame_.post.time = time;

    frame_.post.view_origin = {
      camera.x - (static_cast<float>(RenderConfig::view_width) / 2),
      camera.y - (static_cast<float>(RenderConfig::view_height) / 2),
    };

    frame_.post.view_size = {
      static_cast<float>(RenderConfig::view_width),
      static_cast<float>(RenderConfig::view_height),
    };
  }

  Vec2Props Renderer::viewOrigin() const {
    return Vec2Props::of(
      frame_.post.view_origin[0],
      frame_.post.view_origin[1]
    );
  }

  bool Renderer::isVisible(
    Vec2Props point,
    float     margin
  ) const {
    const Vec2Props origin = viewOrigin();

    return point.x
      > origin.x
      - margin
      && point.y
      > origin.y
      - margin
      && point.x
      < origin.x
      + static_cast<float>(RenderConfig::view_width)
      + margin
      && point.y
      < origin.y
      + static_cast<float>(RenderConfig::view_height)
      + margin;
  }

  QuadProps Renderer::makeQuad(
    Vec2Props center,
    Vec2Props size,
    std::array<
      float,
      4
    >            region,
    ColorProps   color,
    QuadKindEnum kind,
    float        rotation,
    float        parameter
  ) {
    return QuadProps {
      .position_size  = {center.x, center.y, size.x, size.y},
      .texture_region = region,
      .color          = color.toArray(),
      .parameters     = {
                         rotation, static_cast<float>(static_cast<std::uint8_t>(kind)),
                         parameter, 0,
                         },
    };
  }

  std::array<
    float,
    4>
  Renderer::regionCoordinates(
    const AtlasRegionProps &region,
    bool                    mirrored
  ) {
    return mirrored ? region.mirroredTextureCoordinates()
                    : region.textureCoordinates();
  }

  void Renderer::drawSprite(
    const AtlasRegionProps &region,
    Vec2Props               center,
    const SpriteDrawDto    &options
  ) {
    drawSpriteSized(
      region,
      center,
      region.worldSize().scale(options.scale),
      options
    );
  }

  void Renderer::drawSpriteSized(
    const AtlasRegionProps &region,
    Vec2Props               center,
    Vec2Props               size,
    const SpriteDrawDto    &options
  ) {
    const auto coordinates = regionCoordinates(region, options.mirrored);

    frame_.world.push(makeQuad(
      center,
      size,
      coordinates,
      options.color,
      QuadKindEnum::sprite,
      options.rotation,
      options.petrify
    ));

    if(options.glow > 0)
      frame_.glow.push(makeQuad(
        center,
        size,
        coordinates,
        options.color.withAlpha(options.glow),
        QuadKindEnum::sprite,
        options.rotation,
        0
      ));
  }

  void Renderer::drawGlowSprite(
    const AtlasRegionProps &region,
    Vec2Props               center,
    ColorProps              color,
    const SpriteDrawDto    &options
  ) {
    frame_.glow.push(makeQuad(
      center,
      region.worldSize().scale(options.scale),
      regionCoordinates(region, options.mirrored),
      color,
      QuadKindEnum::sprite,
      options.rotation,
      0
    ));
  }

  void Renderer::drawGlowDisc(
    Vec2Props  center,
    float      radius,
    ColorProps color,
    float      softness
  ) {
    frame_.glow.push(makeQuad(
      center,
      Vec2Props::of(radius * 2, radius * 2),
      blank_region,
      color,
      QuadKindEnum::soft_disc,
      0,
      softness
    ));
  }

  void Renderer::drawShadowDisc(
    Vec2Props  center,
    float      radius,
    ColorProps color,
    float      softness
  ) {
    frame_.world.push(makeQuad(
      center,
      Vec2Props::of(radius * 2, radius * 2),
      blank_region,
      color,
      QuadKindEnum::soft_disc,
      0,
      softness
    ));
  }

  void Renderer::drawGlowRectangle(
    Vec2Props  center,
    Vec2Props  size,
    ColorProps color
  ) {
    frame_.glow.push(
      makeQuad(center, size, blank_region, color, QuadKindEnum::solid, 0, 0)
    );
  }

  void Renderer::drawGlowBeam(
    Vec2Props  origin,
    float      angle,
    float      length,
    float      width,
    ColorProps color
  ) {
    const Vec2Props center
      = origin.add(Vec2Props::fromAngle(angle).scale(length / 2));

    frame_.glow.push(makeQuad(
      center,
      Vec2Props::of(length, width),
      blank_region,
      color,
      QuadKindEnum::light_beam,
      angle,
      0
    ));
  }

  void Renderer::addLight(const GpuLightProps &light) {
    if(frame_.light_count >= RenderConfig::max_gpu_lights) return;
    frame_.lights[frame_.light_count] = light;
    frame_.light_count += 1;
  }

  void Renderer::setWallMap(
    std::uint32_t                 width,
    std::uint32_t                 height,
    std::span<const std::uint8_t> walls,
    std::span<const std::uint8_t> shadow_casters,
    std::uint64_t                 version
  ) {
    frame_.post.map_info = {
      static_cast<float>(width),
      static_cast<float>(height),
      RenderConfig::tile_size,
      0,
    };

    if(frame_.map_version == version) return;
    std::ranges::fill(frame_.map_texels, std::uint8_t {0});

    for(std::uint32_t y = 0; y < height; ++y) {
      for(std::uint32_t x = 0; x < width; ++x) {
        const std::size_t cell = (static_cast<std::size_t>(y) * width) + x;
        std::uint8_t     &texel
          = frame_.map_texels
              [((static_cast<std::size_t>(y) * RenderConfig::max_map_size) + x)
               * 4];

        if(walls[cell] != 0) texel = wall_texel;
        else if(shadow_casters[cell] != 0) texel = shadow_caster_texel;
      }
    }

    frame_.map_version = version;
    frame_.map_revision += 1;
  }

  void Renderer::setRoomGlow(
    int        x,
    int        y,
    ColorProps glow
  ) {
    constexpr auto map_size = static_cast<int>(RenderConfig::max_map_size);
    if(x < 0 || y < 0 || x >= map_size || y >= map_size) return;

    const std::size_t texel
      = ((static_cast<std::size_t>(y) * RenderConfig::max_map_size)
         + static_cast<std::size_t>(x))
      * 4;
    const std::array<std::uint8_t, 3> channels {
      toByte(glow.r),
      toByte(glow.g),
      toByte(glow.b),
    };

    for(std::size_t channel = 0; channel < channels.size(); ++channel) {
      std::uint8_t &stored = frame_.map_texels[texel + 1 + channel];
      if(stored == channels[channel]) continue;
      stored = channels[channel];
      frame_.map_revision += 1;
    }
  }

  void Renderer::setViewer(
    Vec2Props position,
    bool      is_enabled
  ) {
    frame_.post.viewer = {position.x, position.y, is_enabled ? 1.0F : 0.0F, 0};
  }

  void Renderer::setPostProcess(
    ColorProps ambient,
    std::array<
      float,
      4
    > effects,
    std::array<
      float,
      4
    > tint
  ) {
    frame_.post.ambient = ambient.withAlpha(1).toArray();
    frame_.post.effects = effects;
    frame_.post.tint    = tint;
  }

  void Renderer::drawUiRectangle(
    float      x,
    float      y,
    float      width,
    float      height,
    ColorProps color
  ) {
    frame_.ui.push(makeQuad(
      Vec2Props::of(x + (width / 2), y + (height / 2)),
      Vec2Props::of(width, height),
      blank_region,
      color,
      QuadKindEnum::solid,
      0,
      0
    ));
  }

  void Renderer::drawUiShape(
    QuadKindEnum kind,
    Vec2Props    center,
    Vec2Props    size,
    ColorProps   color,
    float        rotation,
    float        parameter
  ) {
    frame_.ui.push(
      makeQuad(center, size, blank_region, color, kind, rotation, parameter)
    );
  }

  void Renderer::drawUiSprite(
    const AtlasRegionProps &region,
    Vec2Props               center,
    float                   size,
    ColorProps              color
  ) {
    const float aspect = region.height / std::max(region.width, 1.0F);

    frame_.ui.push(makeQuad(
      center,
      Vec2Props::of(size, size * aspect),
      region.textureCoordinates(),
      color,
      QuadKindEnum::sprite,
      0,
      0
    ));
  }

  float Renderer::measureText(
    std::string_view text,
    float            size
  ) const {
    return typefaces_.measure(TypefaceEnum::body, text, size);
  }

  void Renderer::drawText(
    std::string_view text,
    float            x,
    float            y,
    float            size,
    ColorProps       color,
    TextAlignEnum    alignment
  ) {
    drawTypefaceText(TypefaceEnum::body, text, x, y, size, color, alignment);
  }

  void Renderer::drawDisplayText(
    std::string_view text,
    float            x,
    float            y,
    float            size,
    ColorProps       color,
    TextAlignEnum    alignment
  ) {
    drawTypefaceText(
      TypefaceEnum::display,
      text,
      x + 2,
      y + 3,
      size,
      ColorProps::black().withAlpha(color.a * 0.7F),
      alignment
    );

    drawTypefaceText(TypefaceEnum::display, text, x, y, size, color, alignment);
  }

  void Renderer::drawTypefaceText(
    TypefaceEnum     typeface,
    std::string_view text,
    float            x,
    float            y,
    float            size,
    ColorProps       color,
    TextAlignEnum    alignment
  ) {
    const float scale = size / typefaces_.pixelSize(typeface);
    float       pen_x = x;

    if(alignment == TextAlignEnum::center)
      pen_x = x - (typefaces_.measure(typeface, text, size) / 2);
    if(alignment == TextAlignEnum::right)
      pen_x = x - typefaces_.measure(typeface, text, size);

    const float baseline = y + (size * 0.8F);
    std::size_t cursor   = 0;

    while(cursor < text.size()) {
      const GlyphProps &glyph = typefaces_.glyphFor(
        typeface,
        Utf8Decoder::nextCodepoint(text, cursor)
      );

      if(glyph.region.width > 0) {
        const float width  = glyph.region.width * scale;
        const float height = glyph.region.height * scale;
        const float left   = pen_x + (glyph.bearing_x * scale);
        const float top    = baseline - (glyph.bearing_y * scale);

        frame_.ui.push(makeQuad(
          Vec2Props::of(left + (width / 2), top + (height / 2)),
          Vec2Props::of(width, height),
          glyph.region.textureCoordinates(),
          color,
          QuadKindEnum::glyph,
          0,
          0
        ));
      }

      pen_x += glyph.advance * scale;
    }
  }
} // namespace wheel_of_fear
