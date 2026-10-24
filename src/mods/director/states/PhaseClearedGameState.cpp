#include "mods/director/states/PhaseClearedGameState.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/render/Renderer.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/director/Director.hpp"
#include "mods/director/screens/CenteredCardWidget.hpp"
#include <format>

namespace wheel_of_fear {
  void PhaseClearedGameState::update(
    Director       &director,
    GameContextDto &context
  ) {
    context.clock.setRunning(false);
    if(director.stateSeconds() > Director::cleared_seconds)
      director.startPhase(context, director.phase() + 1);
  }

  void PhaseClearedGameState::mixMusic(GameContextDto &context) const {
    context.sound_player.setMusicMix(0, 0.8F, 0);
  }

  void PhaseClearedGameState::draw(
    const Director &director,
    GameContextDto &context
  ) const {
    CenteredCardWidget::draw(
      context.renderer,
      std::format("SECTOR {:02} CLEARED", director.phase()),
      "DESCENDING DEEPER INTO THE GRID...",
      Palette::success,
      context.clock.realSeconds()
    );
  }
} // namespace wheel_of_fear
