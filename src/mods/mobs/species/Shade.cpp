#include "mods/mobs/species/Shade.hpp"

#include "mods/art/Art.hpp"

namespace wheel_of_fear {

void Shade::think(GameContextDto& context) {
    thinkAsHunter(context);
    if (distanceToPlayer(context) < whisper_distance) vocalize(context, 0.02F);
}

void Shade::onContact(GameContextDto& context) {
    strikeAndRetreat(context, strike_retreat_seconds);
}

void Shade::draw(GameContextDto& context) const {
    const SpriteSheetProps& sprites = context.art.sprites();
    const std::size_t frame = animationFrame(9, sprites.shade_drift.size());
    drawBody(context, sprites.shade_drift[frame], sprites.shade_drift_glow[frame], MobLookDto{.glow_color = Color::hex(0xe8ecff), .glow_strength = visibility()});
}

}
