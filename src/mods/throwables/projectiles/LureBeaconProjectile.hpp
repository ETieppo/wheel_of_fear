#pragma once

#include "mods/throwables/projectiles/Projectile.hpp"

namespace wheel_of_fear {
  class LureBeaconProjectile final: public Projectile {
    public:
      LureBeaconProjectile(
        Vec2Props start,
        Vec2Props target
      )
          : Projectile(
              ItemEnum::lure_beacon,
              start,
              target
            ) {
      }

      void land(
        GameContextDto &context,
        Vec2Props       landing
      ) override;

    protected:
      [[nodiscard]]
      const AtlasRegionProps &
      sprite(const SpriteSheetProps &sprites) const override {
        return sprites.lure_beacon;
      }

    private:
      static constexpr float impact_noise_radius = 260;
  };
} // namespace wheel_of_fear
