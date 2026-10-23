#pragma once

#include "mods/throwables/projectiles/Projectile.hpp"

namespace wheel_of_fear {

class World;

class NeonGrenadeProjectile final : public Projectile {
public:
    NeonGrenadeProjectile(Vec2 start, Vec2 target) : Projectile(ItemEnum::neon_grenade, start, target) {}

    void land(GameContextDto& context, Vec2 landing) override;

protected:
    const AtlasRegionProps& sprite(const SpriteSheetProps& sprites) const override { return sprites.neon_grenade; }
    void drawTrail(Renderer& renderer) const override;

private:
    static constexpr float noise_radius = 520;
    static constexpr float kill_radius = 36;
    static constexpr float spread_tiles = 3.3F;
    static constexpr std::size_t flood_capacity = 96;

    static void floodNeon(World& world, Vec2 origin, float lifetime_seconds);
};

}
