#include "mods/director/states/PlayingGameState.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/input/InputController.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/time/GameClock.hpp"
#include "mods/director/Director.hpp"
#include "mods/generators/GeneratorSystem.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/player/PlayerEntity.hpp"

namespace wheel_of_fear {
  void PlayingGameState::update(
    Director       &director,
    GameContextDto &context
  ) {
    context.clock.setRunning(true);

    if(director.demo().isActive())
      director.demo().run(context, director.stateSeconds());
    if(context.input.wasPressed(InputActionEnum::back)) {
      director.enterState(GameStateEnum::paused);
      context.clock.setRunning(false);

      return;
    }

    if(context.player.isDead()) {
      director.addDeathSeconds(context.clock.realDeltaSeconds());

      if(director.deathSeconds() > Director::game_over_delay_seconds) {
        director.enterState(GameStateEnum::game_over);
        context.clock.setRunning(false);
      }
    }

    if(context.generators.hasPlayerExited()) {
      director.enterState(GameStateEnum::phase_cleared);
      context.clock.setRunning(false);
      context.sound_player.play(SoundEnum::generator_online, 0.8F, 0, 0.7F);
    }
  }

  void PlayingGameState::mixMusic(GameContextDto &context) const {
    context.sound_player.setMusicMix(
      0,
      0.8F,
      ScalarMath::clamp(context.mobs.danger().level() * 1.4F, 0, 1)
    );
  }
} // namespace wheel_of_fear
