#include "mods/director/states/PhaseClearedGameState.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/render/Renderer.hpp"
#include "core/text/Localization.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/director/CampaignConfig.hpp"
#include "mods/director/Director.hpp"
#include "mods/director/screens/CenteredCardWidget.hpp"
#include <format>

namespace wheel_of_fear {
  namespace {
    constexpr LocalizedTextProps cleared_title {
      "SECTOR {:02} CLEARED",
      "SETOR {:02} CONCLUÍDO",
    };
    constexpr LocalizedTextProps final_gate_subtitle {
      "THE LAST GATE OPENS. DAYLIGHT AHEAD...",
      "O ÚLTIMO PORTÃO SE ABRE. HÁ LUZ DO DIA À FRENTE...",
    };
    constexpr LocalizedTextProps descending_subtitle {
      "DESCENDING DEEPER INTO THE GRID...",
      "DESCENDO MAIS FUNDO NA REDE...",
    };
  } // namespace

  void PhaseClearedGameState::update(
    Director       &director,
    GameContextDto &context
  ) {
    context.clock.setRunning(false);
    if(director.stateSeconds() <= Director::cleared_seconds) return;

    director.advanceToNextPhase(context);
  }

  void PhaseClearedGameState::mixMusic(GameContextDto &context) const {
    context.sound_player.setMusicMix(0, 0.8F, 0);
  }

  void PhaseClearedGameState::draw(
    const Director &director,
    GameContextDto &context
  ) const {
    const bool is_final_phase = director.phase() >= CampaignConfig::final_phase;

    CenteredCardWidget::draw(
      context.renderer,
      Localization::format(cleared_title, director.phase()),
      is_final_phase ? Localization::text(final_gate_subtitle)
                     : Localization::text(descending_subtitle),
      Palette::success,
      context.clock.realSeconds()
    );
  }
} // namespace wheel_of_fear
