#include "mods/mobs/species/CrawlerMob.hpp"
#include "mods/art/ArtLibrary.hpp"

namespace wheel_of_fear {
  void CrawlerMob::onContact(GameContextDto &context) {
    brain().strikeAndRetreat(context, strike_retreat_seconds);
  }

  void CrawlerMob::onLightReleased(GameContextDto &context) {
    brain().setRetreatSeconds(flinch_seconds);
    voice().vocalize(context, 0.5F);
  }

  void CrawlerMob::draw(GameContextDto &context) const {
    const SpriteSheetProps &sprites           = context.art.sprites();
    const float             frames_per_second = isMoving() ? 16.0F : 3.0F;
    const std::size_t       frame
      = animationFrame(frames_per_second, sprites.crawler_walk.size());

    appearance().drawBody(
      context,
      sprites.crawler_walk[frame],
      sprites.crawler_walk_glow[frame],
      MobLookDto {
        .glow_color      = ColorProps::hex(0xff2a4a),
        .rotation_offset = sprite_facing_offset,
      }
    );
  }
} // namespace wheel_of_fear
