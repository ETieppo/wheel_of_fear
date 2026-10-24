#include "mods/director/states/PhaseIntroGameState.hpp"

#include "core/audio/SoundPlayer.hpp"
#include "core/input/InputController.hpp"
#include "core/render/Renderer.hpp"
#include "core/time/GameClock.hpp"
#include "mods/director/Director.hpp"
#include "mods/director/screens/PhaseIntroScreen.hpp"
#include "mods/hud/HudSystem.hpp"

namespace wheel_of_fear {
  void PhaseIntroGameState::update(
    Director       &director,
    GameContextDto &context
  ) {
    context.clock.setRunning(false);
    context.hud.setVisible(true);

    const bool is_skipped =
      context.input.wasPressed(InputActionEnum::confirm) ||
      (director.demo().isActive() && director.stateSeconds() > 0.5F);

    if(director.stateSeconds() <= Director::intro_seconds && !is_skipped)
      return;

    director.enterState(GameStateEnum::playing);
    context.clock.setRunning(true);
  }

  void PhaseIntroGameState::mixMusic(
    GameContextDto &context
  ) const {
    context.sound_player.setMusicMix(0, 0.8F, 0);
  }

  void PhaseIntroGameState::draw(
    const Director &director,
    GameContextDto &context
  ) const {
    PhaseIntroScreen::draw(
      context.renderer,
      director.phase(),
      director.introLines(),
      director.stateSeconds(),
      Director::intro_seconds,
      context.clock.realSeconds()
    );
  }
} // namespace wheel_of_fear
