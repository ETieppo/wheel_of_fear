#pragma once

#include "mods/world/scenery/Scenery.hpp"

namespace wheel_of_fear {
  class TerminalScenery final: public Scenery {
    public:
      using Scenery::Scenery;

      [[nodiscard]]
      std::optional<DistractionKindEnum> bumpDistraction() const override {
        return DistractionKindEnum::glass_shatter;
      }

    protected:
      [[nodiscard]]
      const AtlasRegionProps &sprite(
        const SpriteSheetProps &sprites
      ) const override {
        return sprites.terminal;
      }

      [[nodiscard]]
      const AtlasRegionProps* glowSprite(
        const SpriteSheetProps &sprites
      ) const override {
        return &sprites.terminal_glow;
      }

      void reactToBump(
        RandomGenerator &random
      ) override {
        if(random.chance(0.5F)) breakApart();
      }
  };
} // namespace wheel_of_fear
