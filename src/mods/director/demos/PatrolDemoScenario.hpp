#pragma once

#include <cstdint>

#include "mods/director/demos/DemoScenario.hpp"

namespace wheel_of_fear {
  class PatrolDemoScenario final: public DemoScenario {
    public:
      explicit PatrolDemoScenario(
        std::uint32_t starting_phase
      )
          : starting_phase_(starting_phase) {
      }

      [[nodiscard]]
      std::uint32_t startingPhase() const override {
        return starting_phase_;
      }

      void run(
        GameContextDto    &context,
        const DemoTickDto &tick
      ) override;

    private:
      static constexpr float log_step_seconds = 5;

      static void logPatrols(
        GameContextDto &context,
        float           seconds
      );

      std::uint32_t starting_phase_;
  };
} // namespace wheel_of_fear
