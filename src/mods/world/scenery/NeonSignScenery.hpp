#pragma once

#include <cstddef>

#include "mods/world/scenery/Scenery.hpp"

namespace wheel_of_fear {
  class NeonSignScenery final: public Scenery {
    public:
      NeonSignScenery(
        const SceneryPlacementDto &placement,
        std::size_t                variant
      )
          : Scenery(placement)
          , variant_(variant) {
      }

      [[nodiscard]]
      std::optional<DistractionKindEnum> bumpDistraction() const override {
        return std::nullopt;
      }

      void draw(
        Renderer               &renderer,
        const SpriteSheetProps &sprites,
        const SceneryViewDto   &view
      ) const override;
      void emitLight(
        LightingSystem       &lighting,
        const SceneryViewDto &view
      ) const override;

    protected:
      [[nodiscard]]
      const AtlasRegionProps &sprite(
        const SpriteSheetProps &sprites
      ) const override {
        return sprites.neon_signs[variant_];
      }

    private:
      static constexpr float light_radius = 42;

      [[nodiscard]]
      float brightness(
        const SceneryViewDto &view
      ) const;

      std::size_t variant_;
  };
} // namespace wheel_of_fear
