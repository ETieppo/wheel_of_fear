#pragma once

#include "mods/throwables/projectiles/Projectile.hpp"

namespace wheel_of_fear {
  class NeonGrenadeProjectile final: public Projectile {
    public:
      NeonGrenadeProjectile(
        Vec2Props start,
        Vec2Props target
      )
          : Projectile(
              ItemEnum::neon_grenade,
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
        return sprites.neon_grenade;
      }

      void drawTrail(Renderer &renderer) const override;

    private:
      static constexpr float noise_radius = 520;
      static constexpr float kill_radius  = 36;
      static constexpr float spread_tiles = 4;
  };
} // namespace wheel_of_fear
