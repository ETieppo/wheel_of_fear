#pragma once

#include "core/math/Vec2Props.hpp"
#include "core/render/Renderer.hpp"
#include "mods/art/SpriteSheetProps.hpp"
#include "mods/lighting/LightingSystem.hpp"

namespace wheel_of_fear {
  class LetterEntity {
    public:
      static constexpr float reach = 18;

      explicit LetterEntity(Vec2Props position)
          : position_(position) {
      }

      [[nodiscard]]
      Vec2Props position() const {
        return position_;
      }

      void settleOn(Vec2Props floor) {
        position_ = floor;
      }

      void emitSpotlight(
        LightingSystem &lighting,
        float           real_seconds
      ) const;

      void draw(
        Renderer               &renderer,
        const SpriteSheetProps &sprites,
        float                   real_seconds
      ) const;

    private:
      static constexpr float spotlight_height       = 46;
      static constexpr float spotlight_reach        = 96;
      static constexpr float spotlight_intensity    = 2.4F;
      static constexpr float spotlight_outer_cosine = 0.93F;
      static constexpr float spotlight_inner_cosine = 0.985F;
      static constexpr float letter_rotation        = -0.18F;

      [[nodiscard]]
      Vec2Props spotlightOrigin() const;

      Vec2Props position_;
  };
} // namespace wheel_of_fear
