#pragma once

#include "mods/world/scenery/Scenery.hpp"

namespace wheel_of_fear {
  class Barrel final: public Scenery {
    public:
      using Scenery::Scenery;

      std::optional<DistractionKindEnum> bumpDistraction() const override {
        return DistractionKindEnum::metal_clang;
      }

    protected:
      const AtlasRegionProps &
      sprite(const SpriteSheetProps &sprites) const override {
        return sprites.barrel;
      }

      void reactToBump(RandomGenerator &random) override {
        tilt(random.range(-0.35F, 0.35F));
      }
  };
} // namespace wheel_of_fear
