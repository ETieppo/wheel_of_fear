#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

#include "core/math/Color.hpp"
#include "core/math/Vec2.hpp"
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
        Vec2  camera,
        float time
      );
      Vec2 viewOrigin() const;
      bool isVisible(
        Vec2  point,
        float margin
      ) const;

      void drawSprite(
        const AtlasRegionProps &region,
        Vec2                    center,
        const SpriteDrawDto    &options = {}
      );
      void drawSpriteSized(
        const AtlasRegionProps &region,
        Vec2                    center,
        Vec2                    size,
        const SpriteDrawDto    &options
      );
      void drawGlowSprite(
        const AtlasRegionProps &region,
        Vec2                    center,
        Color                   color,
        const SpriteDrawDto    &options = {}
      );
      void drawGlowDisc(
        Vec2  center,
        float radius,
        Color color,
        float softness
      );
      void drawShadowDisc(
        Vec2  center,
        float radius,
        Color color,
        float softness
      );
      void drawGlowRectangle(
        Vec2  center,
        Vec2  size,
        Color color
      );
      void drawGlowBeam(
        Vec2  origin,
        float angle,
        float length,
        float width,
        Color color
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
        Vec2 position,
        bool is_enabled
      );
      void setPostProcess(
        Color ambient,
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
        float x,
        float y,
        float width,
        float height,
        Color color
      );
      void drawUiShape(
        QuadKindEnum kind,
        Vec2         center,
        Vec2         size,
        Color        color,
        float        rotation,
        float        parameter
      );
      void drawUiSprite(
        const AtlasRegionProps &region,
        Vec2                    center,
        float                   size,
        Color                   color
      );
      float measureText(
        std::string_view text,
        float            size
      ) const;
      void drawText(
        std::string_view text,
        float            x,
        float            y,
        float            size,
        Color            color,
        TextAlignEnum    alignment
      );
      void drawDisplayText(
        std::string_view text,
        float            x,
        float            y,
        float            size,
        Color            color,
        TextAlignEnum    alignment
      );

    private:
      static QuadProps makeQuad(
        Vec2 center,
        Vec2 size,
        std::array<
          float,
          4
        >            region,
        Color        color,
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
        Color            color,
        TextAlignEnum    alignment
      );

      TextureAtlas                      atlas_;
      TypefaceSet                       typefaces_;
      FrameProps                        frame_;
      std::optional<GroundMaterialsDto> pending_ground_materials_;
  };
} // namespace wheel_of_fear
