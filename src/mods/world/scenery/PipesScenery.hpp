#pragma once

#include "mods/world/scenery/Scenery.hpp"

namespace wheel_of_fear {
  class PipesScenery final: public Scenery {
    public:
      using Scenery::Scenery;

      [[nodiscard]]
      std::optional<DistractionKindEnum> bumpDistraction() const override {
        return DistractionKindEnum::pipe_rattle;
      }

    protected:
      [[nodiscard]]
      const AtlasRegionProps &sprite(
        const SpriteSheetProps &sprites
      ) const override {
        return sprites.pipes;
      }
  };
} // namespace wheel_of_fear
