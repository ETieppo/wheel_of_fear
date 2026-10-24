#include "mods/mobs/species/ShepherdMob.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void ShepherdMob::think(GameContextDto &context) {
    const float delta = context.clock.deltaSeconds();
    seconds_since_call_ += delta;

    if(!canAct()) return;
    const Vec2Props player_position = context.player.position();

    if(brain().intent() == MobIntentEnum::calling) {
      faceToward(player_position);

      if(brain().intentSeconds() > call_seconds)
        brain().startSearching(position());

      brain().addIntentSeconds(delta);
      return;
    }

    if(
      perception().canSeePlayerNow()
      && seconds_since_call_
      >= call_interval_seconds
    ) {
      call(context);
      return;
    }

    if(
      perception().canSeePlayerNow()
      && distanceToPlayer(context)
      < preferred_distance
    ) {
      locomotion().moveAlong(
        context,
        position().sub(player_position).normalized(),
        locomotion().currentSpeed()
      );

      faceToward(player_position);
      return;
    }

    brain().hunt(context);
  }

  void ShepherdMob::call(GameContextDto &context) {
    seconds_since_call_ = 0;
    brain().changeIntent(MobIntentEnum::calling, position());

    voice().playSound(
      context,
      SoundEnum::shepherd_call,
      1.0F,
      context.world.random().range(0.9F, 1.1F)
    );

    context.noise
      .emit(position(), call_radius * 0.5F, NoiseSourceEnum::shepherd_call);
    context.mobs.answerShepherd(
      position(),
      context.player.position(),
      call_radius,
      call_frenzy_seconds
    );
  }

  void ShepherdMob::draw(GameContextDto &context) const {
    const SpriteSheetProps &sprites = context.art.sprites();
    const std::size_t       frame
      = animationFrame(isMoving() ? 8.0F : 2.0F, sprites.shepherd_walk.size());

    appearance().drawBody(
      context,
      sprites.shepherd_walk[frame],
      sprites.shepherd_walk_glow[frame],
      MobLookDto {.glow_color = ColorProps::hex(0xb06bff)}
    );

    if(
      seconds_since_call_
      >= call_ripple_seconds
      || condition().petrifyAmount()
      >= 0.5F
    )
      return;

    const float progress = seconds_since_call_ / call_ripple_seconds;

    context.renderer.drawGlowSprite(
      sprites.twirl,
      position(),
      ColorProps::hex(0xa060ff).withAlpha(0.4F * (1 - progress)),
      SpriteDrawDto {.rotation = progress * 3, .scale = 1 + (progress * 7)}
    );
  }
} // namespace wheel_of_fear
