#pragma once

#include "game/dtos/GameContextDto.hpp"
#include "mods/director/DemoScenarioRunner.hpp"
#include "mods/director/DirectorSnapshotProps.hpp"
#include "mods/director/ProgressStore.hpp"
#include "mods/director/enums/GameStateEnum.hpp"
#include "mods/director/states/CreditsGameState.hpp"
#include "mods/director/states/GameOverGameState.hpp"
#include "mods/director/states/PausedGameState.hpp"
#include "mods/director/states/PhaseClearedGameState.hpp"
#include "mods/director/states/PhaseIntroGameState.hpp"
#include "mods/director/states/PlayingGameState.hpp"
#include "mods/director/states/TitleGameState.hpp"
#include <cstdint>
#include <filesystem>
#include <optional>

namespace wheel_of_fear {
  class Director {
    public:
      static constexpr float intro_seconds           = 6;
      static constexpr float cleared_seconds         = 3;
      static constexpr float game_over_delay_seconds = 1.4F;
      explicit Director(
        std::optional<std::filesystem::path> progress_file_path
      );
      static void initialize(GameContextDto &context);
      void        update(GameContextDto &context);
      void        draw(GameContextDto &context) const;
      void        enterState(GameStateEnum state);

      [[nodiscard]]
      DirectorSnapshotProps snapshot() const;

      void resume(
        GameContextDto              &context,
        const DirectorSnapshotProps &snapshot
      );

      [[nodiscard]]
      GameStateEnum state() const {
        return state_;
      }

      [[nodiscard]]
      float stateSeconds() const {
        return state_seconds_;
      }

      [[nodiscard]]
      std::uint32_t phase() const {
        return phase_;
      }

      [[nodiscard]]
      float deathSeconds() const {
        return death_seconds_;
      }

      void addDeathSeconds(float seconds) {
        death_seconds_ += seconds;
      }

      [[nodiscard]]
      DemoScenarioRunner &demo() {
        return demo_;
      }

      [[nodiscard]]
      const DemoScenarioRunner &demo() const {
        return demo_;
      }

      [[nodiscard]]
      ProgressStore &progress() {
        return progress_;
      }

      [[nodiscard]]
      const ProgressStore &progress() const {
        return progress_;
      }

      void startPhase(
        GameContextDto &context,
        std::uint32_t   phase
      );

      void startNewRun(
        GameContextDto &context,
        std::uint32_t   phase
      );

      void advanceToNextPhase(GameContextDto &context);

    private:
      static constexpr std::uint64_t title_seed      = 0x7171;
      static constexpr std::uint64_t phase_seed_step = 0x9E3779B97F4A7C15ULL;

      void saveProgress();
      void updateFade(float real_delta_seconds);
      void configurePostProcess(GameContextDto &context) const;

      [[nodiscard]]
      const GameState &currentState() const;

      [[nodiscard]]
      GameState &currentState();

      GameStateEnum state_                = GameStateEnum::title;
      float         state_seconds_        = 0;
      std::uint32_t phase_                = 1;
      std::uint64_t seed_counter_         = 0x5EED;
      int           committed_max_health_ = VitalityComponent::base_max_health;
      float         death_seconds_        = 0;
      float         fade_amount_          = 1;
      DemoScenarioRunner    demo_;
      ProgressStore         progress_;
      TitleGameState        title_state_;
      PhaseIntroGameState   phase_intro_state_;
      PlayingGameState      playing_state_;
      PausedGameState       paused_state_;
      GameOverGameState     game_over_state_;
      PhaseClearedGameState phase_cleared_state_;
      CreditsGameState      credits_state_;
  };
} // namespace wheel_of_fear
