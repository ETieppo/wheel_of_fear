#pragma once

#include "mods/throwables/projectiles/Projectile.hpp"

namespace wheel_of_fear {
  class WorldSystem;

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
      const AtlasRegionProps &sprite(
        const SpriteSheetProps &sprites
      ) const override {
        return sprites.neon_grenade;
      }

      void drawTrail(
        Renderer &renderer
      ) const override;

    private:
      static constexpr float       noise_radius   = 520;
      static constexpr float       kill_radius    = 36;
      static constexpr float       spread_tiles   = 3.3F;
      static constexpr std::size_t flood_capacity = 96;

      static void floodNeon(
        WorldSystem &world,
        Vec2Props    origin,
        float        lifetime_seconds
      );
  };
} // namespace wheel_of_fear
