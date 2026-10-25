#include "mods/director/states/PausedGameState.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/input/InputController.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/Renderer.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/director/Director.hpp"
#include "mods/director/screens/CenteredCardWidget.hpp"
#include "mods/director/screens/MenuButtonCatalog.hpp"
#include "mods/director/screens/MenuButtonWidget.hpp"
#include "mods/mobs/MobSystem.hpp"

namespace wheel_of_fear {
  void PausedGameState::update(
    Director       &director,
    GameContextDto &context
  ) {
    context.clock.setRunning(false);

    if(
      context.input.wasPressed(InputActionEnum::back)
      || context.input.wasPressed(InputActionEnum::confirm)
    ) {
      director.enterState(GameStateEnum::playing);
      context.clock.setRunning(true);
    }

    if(context.input.wasPressed(InputActionEnum::restart))
      director.startPhase(context, director.phase());

    if(
      MenuButtonWidget::wasActivated(
        MenuButtonCatalog::paused_quit,
        context.input
      )
    )
      context.input.requestQuit();
  }

  void PausedGameState::mixMusic(GameContextDto &context) const {
    context.sound_player.setMusicMix(
      0,
      0.8F,
      ScalarMath::clamp(context.mobs.danger().level() * 1.4F, 0, 1)
    );
  }

  void PausedGameState::draw(
    [[maybe_unused]] const Director &director,
    GameContextDto                  &context
  ) const {
    CenteredCardWidget::draw(
      context.renderer,
      "PAUSED",
      "ESC  RESUME     R  RESTART SECTOR     M  MUTE",
      Palette::panel_edge,
      context.clock.realSeconds()
    );

    MenuButtonWidget::draw(
      context.renderer,
      MenuButtonCatalog::paused_quit,
      context.input.mouseInUi()
    );
  }
} // namespace wheel_of_fear
