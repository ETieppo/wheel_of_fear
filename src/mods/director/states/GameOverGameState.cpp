#include "mods/director/states/GameOverGameState.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/input/InputController.hpp"
#include "core/render/Renderer.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/camera/CameraSystem.hpp"
#include "mods/director/Director.hpp"
#include "mods/director/screens/CenteredCardWidget.hpp"

namespace wheel_of_fear {
  void GameOverGameState::update(
    Director       &director,
    GameContextDto &context
  ) {
    context.clock.setRunning(false);

    if(
      context.input.wasPressed(InputActionEnum::restart)
      || context.input.wasPressed(InputActionEnum::confirm)
    )
      director.startPhase(context, director.phase());

    if(context.input.wasPressed(InputActionEnum::back)) {
      director.enterState(GameStateEnum::title);
      context.camera.stopFollowing();
    }
  }

  void GameOverGameState::mixMusic(GameContextDto &context) const {
    context.sound_player.setMusicMix(0, 0.5F, 0);
  }

  void GameOverGameState::draw(
    [[maybe_unused]] const Director &director,
    GameContextDto                  &context
  ) const {
    CenteredCardWidget::draw(
      context.renderer,
      "SIGNAL LOST",
      "R  TRY AGAIN     ESC  TITLE SCREEN",
      Palette::danger,
      context.clock.realSeconds()
    );
  }
} // namespace wheel_of_fear
