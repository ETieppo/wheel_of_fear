#include "mods/director/states/ReadingGameState.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/input/InputController.hpp"
#include "core/text/Localization.hpp"
#include "core/time/GameClock.hpp"
#include "mods/director/Director.hpp"
#include "mods/director/screens/LetterScreen.hpp"
#include "mods/letters/LetterCatalog.hpp"
#include "mods/letters/LetterSystem.hpp"

namespace wheel_of_fear {
  void ReadingGameState::update(
    Director       &director,
    GameContextDto &context
  ) {
    context.clock.setRunning(false);
    if(director.stateSeconds() < close_unlock_seconds) return;

    const InputController &input = context.input;
    if(
      !input.wasPressed(InputActionEnum::interact)
      && !input.wasPressed(InputActionEnum::confirm)
      && !input.wasPressed(InputActionEnum::back)
    )
      return;

    context.letters.finishReading(context);
    director.enterState(GameStateEnum::playing);
    context.clock.setRunning(true);
  }

  void ReadingGameState::mixMusic(GameContextDto &context) const {
    context.sound_player.setMusicMix(0, 0.45F, 0);
  }

  void ReadingGameState::draw(
    const Director &director,
    GameContextDto &context
  ) const {
    LetterScreen::draw(
      context.renderer,
      director.phase(),
      context.letters.textFor(Localization::language()),
      Localization::text(LetterCatalog::giftNoteFor(director.phase())),
      context.clock.realSeconds()
    );
  }
} // namespace wheel_of_fear
