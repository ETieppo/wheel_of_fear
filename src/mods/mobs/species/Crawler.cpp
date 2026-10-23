#include "mods/mobs/species/Crawler.hpp"

#include "mods/art/Art.hpp"

namespace wheel_of_fear {
  void Crawler::onContact(GameContextDto &context) {
    strikeAndRetreat(context, strike_retreat_seconds);
  }

  void Crawler::onLightReleased(GameContextDto &context) {
    setRetreatSeconds(flinch_seconds);
    vocalize(context, 0.5F);
  }

  void Crawler::draw(GameContextDto &context) const {
    const SpriteSheetProps &sprites           = context.art.sprites();
    const float             frames_per_second = isMoving() ? 16.0F : 3.0F;
    const std::size_t       frame =
      animationFrame(frames_per_second, sprites.crawler_walk.size());
    drawBody(
      context,
      sprites.crawler_walk[frame],
      sprites.crawler_walk_glow[frame],
      MobLookDto {.glow_color = Color::hex(0xff2a4a)}
    );
  }
} // namespace wheel_of_fear
