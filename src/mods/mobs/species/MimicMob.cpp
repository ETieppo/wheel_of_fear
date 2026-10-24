#include "mods/mobs/species/MimicMob.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <cmath>

namespace wheel_of_fear {
  void MimicMob::think(GameContextDto &context) {
    if(isBusy()) return;

    switch(stance_) {
      case MimicStanceEnum::disguised: waitInDisguise(context); break;
      case MimicStanceEnum::lunging:   lunge(context); break;
      case MimicStanceEnum::hunting:   brain().hunt(context); break;
    }
  }

  void MimicMob::waitInDisguise(GameContextDto &context) {
    condition().setFrozen(false);

    if(
      distanceToPlayer(context)
      > wake_distance
      || !context.player.canBeTargeted()
    )
      return;

    stance_        = MimicStanceEnum::lunging;
    lunge_seconds_ = lunge_duration;
    voice().playSound(context, SoundEnum::jaws, 1.0F, 1);
    context.noise.emit(position(), 260, NoiseSourceEnum::mob_incident);
    context.world.shake(0.35F);
  }

  void MimicMob::lunge(GameContextDto &context) {
    if(condition().isFrozen()) {
      stance_ = MimicStanceEnum::hunting;
      return;
    }

    locomotion()
      .moveAlong(context, directionTo(context.player.position()), lunge_speed);
    lunge_seconds_ -= context.clock.deltaSeconds();

    if(lunge_seconds_ <= 0) stance_ = MimicStanceEnum::hunting;
    if(perception().touchesPlayer(context, species_traits.strike_reach)) {
      brain().strikeAndRetreat(context, strike_retreat_seconds);
      stance_ = MimicStanceEnum::hunting;
    }
  }

  void MimicMob::onContact(GameContextDto &context) {
    brain().strikeAndRetreat(context, strike_retreat_seconds);
  }

  void MimicMob::draw(GameContextDto &context) const {
    const SpriteSheetProps &sprites  = context.art.sprites();
    Renderer               &renderer = context.renderer;

    if(isDisguised()) {
      renderer.drawShadowDisc(
        position().add(Vec2Props::of(1, 4)),
        12,
        ColorProps::black().withAlpha(0.5F),
        1.3F
      );

      renderer.drawSprite(sprites.fabricator, position());
      const float blink
        = 0.6F + (0.4F * std::sin(context.clock.realSeconds() * 4));

      renderer.drawGlowSprite(
        sprites.fabricator_glow,
        position(),
        ColorProps::hex(0xff2a4a).withAlpha(blink)
      );

      return;
    }

    const std::size_t frame = animationFrame(10, sprites.mimic_hunt.size());

    appearance().drawBody(
      context,
      sprites.mimic_hunt[frame],
      sprites.mimic_hunt_glow[frame],
      MobLookDto {.glow_color = ColorProps::hex(0xffd84a)}
    );
  }
} // namespace wheel_of_fear
