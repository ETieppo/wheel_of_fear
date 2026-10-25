#include "mods/director/states/CreditsGameState.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/input/InputController.hpp"
#include "core/time/GameClock.hpp"
#include "mods/credits/CreditsCatalog.hpp"
#include "mods/director/Director.hpp"
#include "mods/director/screens/CreditsScreen.hpp"
#include "mods/hud/HudSystem.hpp"

namespace wheel_of_fear {
  CreditsGameState::CreditsGameState()
      : entries_(CreditsCatalog::load()) {
  }

  void CreditsGameState::update(
    Director       &director,
    GameContextDto &context
  ) {
    context.clock.setRunning(false);
    context.hud.setVisible(false);

    const InputController &input = context.input;
    if(
      !input.wasPressed(InputActionEnum::back)
      && !input.wasPressed(InputActionEnum::confirm)
      && !input.wasPressed(InputActionEnum::show_credits)
      && !input.wasPressed(InputActionEnum::hug)
    )
      return;

    context.sound_player.play(SoundEnum::ui_glitch, 0.6F, 0, 1);
    director.enterState(GameStateEnum::title);
  }

  void CreditsGameState::mixMusic(GameContextDto &context) const {
    context.sound_player.setMusicMix(1, 0, 0);
  }

  void CreditsGameState::draw(
    [[maybe_unused]] const Director &director,
    GameContextDto                  &context
  ) const {
    CreditsScreen::draw(
      context.renderer,
      entries_,
      context.clock.realSeconds()
    );
  }
} // namespace wheel_of_fear
