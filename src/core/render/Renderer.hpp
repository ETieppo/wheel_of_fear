#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

#include "core/math/ColorProps.hpp"
#include "core/math/Vec2Props.hpp"
#include "core/render/AtlasRegionProps.hpp"
#include "core/render/FrameProps.hpp"
#include "core/render/GpuLightProps.hpp"
#include "core/render/TextureAtlas.hpp"
#include "core/render/TypefaceSet.hpp"
#include "core/render/dtos/GroundMaterialsDto.hpp"
#include "core/render/dtos/SpriteDrawDto.hpp"
#include "core/render/enums/QuadKindEnum.hpp"
#include "core/render/enums/TextAlignEnum.hpp"

namespace wheel_of_fear {
  class Renderer {
    public:
      Renderer();

      TextureAtlas &atlas() {
        return atlas_;
      }

      [[nodiscard]]
      const FrameProps &frame() const {
        return frame_;
      }

      FrameProps &frame() {
        return frame_;
      }

      void setGroundMaterials(
        GroundMaterialsDto materials
      ) {
        pending_ground_materials_ = std::move(materials);
      }

      std::optional<GroundMaterialsDto> takeGroundMaterials();

      void beginFrame(
        Vec2Props camera,
        float     time
      );
      [[nodiscard]]
      Vec2Props viewOrigin() const;
      [[nodiscard]]
      bool isVisible(
        Vec2Props point,
        float     margin
      ) const;

      void drawSprite(
        const AtlasRegionProps &region,
        Vec2Props               center,
        const SpriteDrawDto    &options = {}
      );
      void drawSpriteSized(
        const AtlasRegionProps &region,
        Vec2Props               center,
        Vec2Props               size,
        const SpriteDrawDto    &options
      );
      void drawGlowSprite(
        const AtlasRegionProps &region,
        Vec2Props               center,
        ColorProps              color,
        const SpriteDrawDto    &options = {}
      );
      void drawGlowDisc(
        Vec2Props  center,
        float      radius,
        ColorProps color,
        float      softness
      );
      void drawShadowDisc(
        Vec2Props  center,
        float      radius,
        ColorProps color,
        float      softness
      );
      void drawGlowRectangle(
        Vec2Props  center,
        Vec2Props  size,
        ColorProps color
      );
      void drawGlowBeam(
        Vec2Props  origin,
        float      angle,
        float      length,
        float      width,
        ColorProps color
      );
      void addLight(
        const GpuLightProps &light
      );
      void setWallMap(
        std::uint32_t                 width,
        std::uint32_t                 height,
        std::span<const std::uint8_t> walls,
        std::uint64_t                 version
      );
      void setViewer(
        Vec2Props position,
        bool      is_enabled
      );
      void setPostProcess(
        ColorProps ambient,
        std::array<
          float,
          4
        > effects,
        std::array<
          float,
          4
        > tint
      );

      void drawUiRectangle(
        float      x,
        float      y,
        float      width,
        float      height,
        ColorProps color
      );
      void drawUiShape(
        QuadKindEnum kind,
        Vec2Props    center,
        Vec2Props    size,
        ColorProps   color,
        float        rotation,
        float        parameter
      );
      void drawUiSprite(
        const AtlasRegionProps &region,
        Vec2Props               center,
        float                   size,
        ColorProps              color
      );
      [[nodiscard]]
      float measureText(
        std::string_view text,
        float            size
      ) const;
      void drawText(
        std::string_view text,
        float            x,
        float            y,
        float            size,
        ColorProps       color,
        TextAlignEnum    alignment
      );
      void drawDisplayText(
        std::string_view text,
        float            x,
        float            y,
        float            size,
        ColorProps       color,
        TextAlignEnum    alignment
      );

    private:
      static QuadProps makeQuad(
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
      );
      static std::array<
        float,
        4
      >
           regionCoordinates(
             const AtlasRegionProps &region,
             bool                    mirrored
           );
      void drawTypefaceText(
        TypefaceEnum     typeface,
        std::string_view text,
        float            x,
        float            y,
        float            size,
        ColorProps       color,
        TextAlignEnum    alignment
      );

      TextureAtlas                      atlas_;
      TypefaceSet                       typefaces_;
      FrameProps                        frame_;
      std::optional<GroundMaterialsDto> pending_ground_materials_;
  };
} // namespace wheel_of_fear
