#pragma once

#include "mods/world/scenery/Scenery.hpp"

namespace wheel_of_fear {
  class ServerRack final: public Scenery {
    public:
      using Scenery::Scenery;

      std::optional<DistractionKindEnum> bumpDistraction() const override {
        return DistractionKindEnum::power_surge;
      }

    protected:
      const AtlasRegionProps &
      sprite(const SpriteSheetProps &sprites) const override {
        return sprites.server_rack;
      }

      const AtlasRegionProps*
      glowSprite(const SpriteSheetProps &sprites) const override {
        return &sprites.server_rack_glow;
      }

      void reactToBump(RandomGenerator &random) override {
        if(random.chance(0.5F)) breakApart();
      }
  };
} // namespace wheel_of_fear
