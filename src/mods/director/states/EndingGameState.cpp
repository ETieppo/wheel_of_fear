#include "mods/director/states/EndingGameState.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/input/InputController.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/text/Localization.hpp"
#include "core/time/GameClock.hpp"
#include "mods/credits/CreditsCatalog.hpp"
#include "mods/credits/CreditsRollBuilder.hpp"
#include "mods/director/Director.hpp"
#include "mods/director/screens/CreditsRollWidget.hpp"
#include "mods/hud/HudSystem.hpp"
#include "mods/player/PlayerEntity.hpp"

namespace wheel_of_fear {
  EndingGameState::EndingGameState()
      : entries_(CreditsCatalog::load())
      , lines_(
          CreditsRollBuilder::build(
            entries_,
            CreditsRollStyleEnum::ending
          )
        )
      , lines_language_(Localization::language())
      , roll_height_(CreditsRollBuilder::totalHeight(lines_)) {
  }

  void EndingGameState::rebuildLinesIfLanguageChanged() {
    if(lines_language_ == Localization::language()) return;

    lines_ = CreditsRollBuilder::build(entries_, CreditsRollStyleEnum::ending);
    lines_language_ = Localization::language();
    roll_height_    = CreditsRollBuilder::totalHeight(lines_);
  }

  void EndingGameState::update(
    Director       &director,
    GameContextDto &context
  ) {
    rebuildLinesIfLanguageChanged();
    context.clock.setRunning(true);
    context.hud.setVisible(false);
    context.player.flashlight().setEnabled(false);

    if(director.demo().isActive())
      director.demo().run(context, director.stateSeconds());

    const InputController &input             = context.input;
    const bool             is_skip_requested = director.stateSeconds()
      > skip_unlock_seconds
      && (input.wasPressed(InputActionEnum::confirm)
          || input.wasPressed(InputActionEnum::back));

    if(!is_skip_requested && !isRollFinished(director.stateSeconds())) return;

    context.sound_player.play(SoundEnum::ui_glitch, 0.6F, 0, 1);
    director.returnToTitle(context);
  }

  void EndingGameState::mixMusic(GameContextDto &context) const {
    context.sound_player.setMusicMix(1, 0, 0);
  }

  void EndingGameState::draw(
    const Director &director,
    GameContextDto &context
  ) const {
    const float state_seconds = director.stateSeconds();
    if(state_seconds < credits_delay_seconds) return;

    const float fade = ScalarMath::clamp(
      (state_seconds - credits_delay_seconds) / backdrop_fade_seconds,
      0,
      1
    );

    CreditsRollWidget::draw(
      context.renderer,
      lines_,
      rollScroll(state_seconds),
      backdrop_alpha * fade
    );
  }

  float EndingGameState::rollScroll(float state_seconds) {
    return (state_seconds - credits_delay_seconds) * scroll_pixels_per_second;
  }

  bool EndingGameState::isRollFinished(float state_seconds) const {
    return state_seconds
      > credits_delay_seconds
      && rollScroll(state_seconds)
      > roll_height_
      + RenderConfig::ui_height;
  }
} // namespace wheel_of_fear
