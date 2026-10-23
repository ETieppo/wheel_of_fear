#include "mods/throwables/projectiles/NeonGrenadeProjectile.hpp"

#include <array>
#include <vector>

#include "core/audio/SoundPlayer.hpp"
#include "core/math/Scalar.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/theme/Palette.hpp"
#include "mods/effects/Effects.hpp"
#include "mods/mobs/Mobs.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/player/Player.hpp"
#include "mods/throwables/NeonPuddle.hpp"
#include "mods/throwables/Throwables.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {

void NeonGrenadeProjectile::land(GameContextDto& context, Vec2 landing) {
    World& world = context.world;
    const Vec2 player_position = context.player.position();
    context.noise.emit(landing, noise_radius, NoiseSourceEnum::explosion);
    context.sound_player.playAt(SoundEnum::neon_burst, 1.0F, player_position, landing, world.random().range(0.92F, 1.05F));
    context.effects.emitNeonBurst(world, landing);
    world.shake(Scalar::clamp(0.8F - landing.distance(player_position) / 300, 0.1F, 0.6F));
    context.mobs.destroyInBlast(context, landing, kill_radius);
    floodNeon(world, landing, NeonPuddle::lifetime_seconds);
    context.throwables.spillPuddle(landing);
}

void NeonGrenadeProjectile::floodNeon(World& world, Vec2 origin, float lifetime_seconds) {
    constexpr std::array<TileCoordinateProps, 4> steps{{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
    TileMap& map = world.map();
    RandomGenerator& random = world.random();
    const TileCoordinateProps start = TileMap::tileAt(origin);
    std::vector<TileCoordinateProps> queue{start};
    map.spillNeon(start.x, start.y, lifetime_seconds + random.range(0, 6));
    for (std::size_t head = 0; head < queue.size(); ++head) {
        const TileCoordinateProps current = queue[head];
        for (const TileCoordinateProps& step : steps) {
            const int x = current.x + step.x;
            const int y = current.y + step.y;
            if (map.isWall(x, y) || map.neonAtTile(x, y) > 0) continue;
            const Vec2 offset = TileMap::tileCenter(x, y).sub(TileMap::tileCenter(start.x, start.y));
            const float reach = spread_tiles * RenderConfig::tile_size * (0.75F + 0.35F * random.unit());
            if (offset.length() > reach || queue.size() >= flood_capacity) continue;
            map.spillNeon(x, y, lifetime_seconds + random.range(-4, 6));
            queue.push_back({x, y});
        }
    }
}

void NeonGrenadeProjectile::drawTrail(Renderer& renderer) const {
    renderer.drawGlowDisc(airPosition(), 5, Palette::neon_magenta.withAlpha(0.6F), 1.4F);
}

}
