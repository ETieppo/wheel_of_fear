#include "mods/director/states/CreditsGameState.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/input/InputController.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/text/Localization.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/credits/CreditsCatalog.hpp"
#include "mods/credits/CreditsRollBuilder.hpp"
#include "mods/director/Director.hpp"
#include "mods/director/screens/CreditsRollWidget.hpp"
#include "mods/hud/HudSystem.hpp"
#include <cmath>

namespace wheel_of_fear {
  namespace {
    constexpr LocalizedTextProps return_hint {
      "PRESS ESC TO RETURN",
      "APERTE ESC PARA VOLTAR",
    };
  } // namespace

  CreditsGameState::CreditsGameState()
      : entries_(CreditsCatalog::load())
      , lines_(
          CreditsRollBuilder::build(
            entries_,
            CreditsRollStyleEnum::title_menu
          )
        )
      , lines_language_(Localization::language())
      , roll_height_(CreditsRollBuilder::totalHeight(lines_)) {
  }

  void CreditsGameState::rebuildLinesIfLanguageChanged() {
    if(lines_language_ == Localization::language()) return;

    lines_
      = CreditsRollBuilder::build(entries_, CreditsRollStyleEnum::title_menu);
    lines_language_ = Localization::language();
    roll_height_    = CreditsRollBuilder::totalHeight(lines_);
  }

  void CreditsGameState::update(
    Director       &director,
    GameContextDto &context
  ) {
    rebuildLinesIfLanguageChanged();
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
    const Director &director,
    GameContextDto &context
  ) const {
    Renderer   &renderer    = context.renderer;
    const float loop_length = roll_height_ + RenderConfig::ui_height;
    const float scroll      = std::fmod(
      director.stateSeconds() * scroll_pixels_per_second,
      loop_length
    );

    CreditsRollWidget::draw(renderer, lines_, scroll, backdrop_alpha);

    renderer.drawUiRectangle(
      0,
      RenderConfig::ui_height - footer_height,
      RenderConfig::ui_width,
      footer_height,
      Palette::panel_deep
    );

    const float pulse
      = 0.45F + (0.55F * std::sin(context.clock.realSeconds() * 3));

    renderer.drawDisplayText(
      Localization::text(return_hint),
      RenderConfig::ui_width / 2,
      RenderConfig::ui_height - footer_height + 12,
      18,
      Palette::accent.withAlpha(pulse),
      TextAlignEnum::center
    );
  }
} // namespace wheel_of_fear
