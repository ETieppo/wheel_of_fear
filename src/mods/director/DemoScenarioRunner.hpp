#pragma once

#include "game/dtos/GameContextDto.hpp"
#include "mods/director/demos/DemoScenario.hpp"
#include <cstdint>
#include <memory>

namespace wheel_of_fear {
  class DemoScenarioRunner {
    public:
      DemoScenarioRunner();
      void restart();

      [[nodiscard]]
      bool isActive() const {
        return scenario_ != nullptr;
      }

      [[nodiscard]]
      std::uint32_t startingPhase() const {
        return scenario_ ? scenario_->startingPhase() : 1;
      }

      void run(
        GameContextDto &context,
        float           seconds
      );

    private:
      static constexpr float log_interval_seconds = 0.5F;

      std::unique_ptr<DemoScenario> scenario_;
      float                         log_timer_ = 0;
  };
} // namespace wheel_of_fear
