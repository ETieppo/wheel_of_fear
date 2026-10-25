#pragma once

#include "mods/world/scenery/Scenery.hpp"

namespace wheel_of_fear {
  class TreeScenery final: public Scenery {
    public:
      explicit TreeScenery(const SceneryPlacementDto &placement)
          : Scenery(placement)
          , variant_(placement.variant) {
      }

      [[nodiscard]]
      std::optional<DistractionKindEnum> bumpDistraction() const override {
        return std::nullopt;
      }

    protected:
      [[nodiscard]]
      const AtlasRegionProps &
      sprite(const SpriteSheetProps &sprites) const override {
        return sprites.trees[variant_ % sprites.trees.size()];
      }

    private:
      std::size_t variant_;
  };
} // namespace wheel_of_fear
