#include "mods/mobs/species/Mimic.hpp"

#include <cmath>

#include "core/time/GameClock.hpp"
#include "mods/art/Art.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/player/Player.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  void Mimic::think(GameContextDto &context) {
    if(isBusy()) return;
    switch(stance_) {
      case MimicStanceEnum::disguised: waitInDisguise(context); break;
      case MimicStanceEnum::lunging:   lunge(context); break;
      case MimicStanceEnum::hunting:   thinkAsHunter(context); break;
    }
  }

  void Mimic::waitInDisguise(GameContextDto &context) {
    setFrozen(false);
    if(distanceToPlayer(context) >
       wake_distance ||
       !context.player.canBeTargeted())
      return;
    stance_        = MimicStanceEnum::lunging;
    lunge_seconds_ = lunge_duration;
    playSound(context, SoundEnum::jaws, 1.0F, 1);
    context.noise.emit(position(), 260, NoiseSourceEnum::mob_incident);
    context.world.shake(0.35F);
  }

  void Mimic::lunge(GameContextDto &context) {
    if(isFrozen()) {
      stance_ = MimicStanceEnum::hunting;
      return;
    }
    moveAlong(context, directionTo(context.player.position()), lunge_speed);
    lunge_seconds_ -= context.clock.deltaSeconds();
    if(lunge_seconds_ <= 0) stance_ = MimicStanceEnum::hunting;
    if(touchesPlayer(context, species_traits.strike_reach)) {
      strikeAndRetreat(context, strike_retreat_seconds);
      stance_ = MimicStanceEnum::hunting;
    }
  }

  void Mimic::onContact(GameContextDto &context) {
    strikeAndRetreat(context, strike_retreat_seconds);
  }

  void Mimic::draw(GameContextDto &context) const {
    const SpriteSheetProps &sprites  = context.art.sprites();
    Renderer               &renderer = context.renderer;
    if(isDisguised()) {
      renderer.drawShadowDisc(
        position().add(Vec2::of(1, 4)), 12, Color::black().withAlpha(0.5F), 1.3F
      );
      renderer.drawSprite(sprites.fabricator, position());
      const float blink =
        0.6F + 0.4F * std::sin(context.clock.realSeconds() * 4);
      renderer.drawGlowSprite(
        sprites.fabricator_glow,
        position(),
        Color::hex(0xff2a4a).withAlpha(blink)
      );
      return;
    }
    const std::size_t frame = animationFrame(10, sprites.mimic_hunt.size());
    drawBody(
      context,
      sprites.mimic_hunt[frame],
      sprites.mimic_hunt_glow[frame],
      MobLookDto {.glow_color = Color::hex(0xffd84a)}
    );
  }
} // namespace wheel_of_fear
