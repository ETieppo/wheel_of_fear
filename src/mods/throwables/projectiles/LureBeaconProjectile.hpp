#pragma once

#include "mods/throwables/projectiles/Projectile.hpp"

namespace wheel_of_fear {
  class LureBeaconProjectile final: public Projectile {
    public:
      LureBeaconProjectile(
        Vec2 start,
        Vec2 target
      )
          : Projectile(
              ItemEnum::lure_beacon,
              start,
              target
            ) {
      }

      void land(
        GameContextDto &context,
        Vec2            landing
      ) override;

    protected:
      const AtlasRegionProps &
      sprite(const SpriteSheetProps &sprites) const override {
        return sprites.lure_beacon;
      }

    private:
      static constexpr float impact_noise_radius = 260;
  };
} // namespace wheel_of_fear
