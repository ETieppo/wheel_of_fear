#include "mods/mobs/species/ShadeMob.hpp"
#include "mods/art/ArtLibrary.hpp"

namespace wheel_of_fear {
  void ShadeMob::think(GameContextDto &context) {
    brain().hunt(context);

    if(distanceToPlayer(context) < whisper_distance)
      voice().vocalize(context, 0.02F);
  }

  void ShadeMob::onContact(GameContextDto &context) {
    brain().strikeAndRetreat(context, strike_retreat_seconds);
  }

  void ShadeMob::draw(GameContextDto &context) const {
    const SpriteSheetProps &sprites = context.art.sprites();
    const std::size_t frame = animationFrame(9, sprites.shade_drift.size());

    appearance().drawBody(
      context,
      sprites.shade_drift[frame],
      sprites.shade_drift_glow[frame],
      MobLookDto {
        .glow_color    = ColorProps::hex(0xe8ecff),
        .glow_strength = appearance().visibility(),
      }
    );
  }
} // namespace wheel_of_fear
