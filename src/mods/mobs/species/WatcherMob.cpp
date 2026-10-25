#include "mods/mobs/species/WatcherMob.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void WatcherMob::think(GameContextDto &context) {
    seconds_since_scream_ += context.clock.deltaSeconds();
    if(!canAct()) return;

    if(perception().isLured() || !perception().canSeePlayerNow()) {
      brain().hunt(context);
      return;
    }

    keepWatching(context);
    if(seconds_since_scream_ >= scream_cooldown_seconds) scream(context);
  }

  void WatcherMob::keepWatching(GameContextDto &context) {
    const Vec2Props player_position = context.player.position();
    faceToward(player_position);
    if(distanceToPlayer(context) >= keep_away_distance) return;

    locomotion().moveAlong(
      context,
      position().sub(player_position).normalized(),
      locomotion().currentSpeed()
    );
    faceToward(player_position);
  }

  void WatcherMob::scream(GameContextDto &context) {
    seconds_since_scream_ = 0;
    voice().playSound(
      context,
      SoundEnum::distant_scream,
      1.0F,
      context.world.random().range(0.85F, 1.0F)
    );

    context.noise
      .emit(position(), keep_away_distance * 3, NoiseSourceEnum::shepherd_call);
    context.mobs.answerCall(
      MobKindEnum::watcher,
      position(),
      context.player.position(),
      sector_wide_radius,
      scream_frenzy_seconds
    );
  }

  void WatcherMob::draw(GameContextDto &context) const {
    const SpriteSheetProps &sprites = context.art.sprites();
    const std::size_t       frame
      = animationFrame(isMoving() ? 7.0F : 3.0F, sprites.watcher_stare.size());
    const bool is_screaming = seconds_since_scream_ < scream_flash_seconds;

    appearance().drawBody(
      context,
      sprites.watcher_stare[frame],
      MobLookDto {
        .tint = is_screaming ? ColorProps::hex(0xff8a8a) : ColorProps::white(),
      }
    );
  }
} // namespace wheel_of_fear
