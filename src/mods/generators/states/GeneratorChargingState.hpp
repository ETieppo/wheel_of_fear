#pragma once

#include "mods/generators/enums/SkillCheckResultEnum.hpp"
#include "mods/generators/states/GeneratorState.hpp"

namespace wheel_of_fear {
  class GeneratorChargingState final: public GeneratorState {
    public:
      [[nodiscard]]
      ColorProps statusColor(
        const GeneratorEntity &generator
      ) const override;

      [[nodiscard]]
      bool canBeRepaired() const override {
        return true;
      }

      void update(
        GeneratorEntity          &generator,
        GameContextDto           &context,
        const GeneratorUpdateDto &update
      ) override;

    private:
      static constexpr float regression_delay_seconds = 2.5F;
      static constexpr float regression_per_second    = 0.025F;
      static constexpr float failure_regression       = 0.14F;
      static constexpr float perfect_bonus            = 0.05F;
      static constexpr float failure_breakdown_chance = 0.3F;

      void idle(
        GeneratorEntity          &generator,
        GameContextDto           &context,
        const GeneratorUpdateDto &update
      );
      void charge(
        GeneratorEntity          &generator,
        GameContextDto           &context,
        const GeneratorUpdateDto &update
      );
      void playRatchet(
        GeneratorEntity &generator,
        GameContextDto  &context,
        float            delta_seconds
      );
      void regressWhileUnattended(
        GeneratorEntity &generator,
        GameContextDto  &context
      );
      static void updateSkillCheck(
        GeneratorEntity &generator,
        GameContextDto  &context
      );
      static void resolveSkillCheck(
        GeneratorEntity     &generator,
        GameContextDto      &context,
        SkillCheckResultEnum result
      );
      static void shortCircuit(
        GeneratorEntity &generator,
        GameContextDto  &context
      );

      float seconds_unattended_    = 0;
      float ratchet_timer_         = 0;
      bool  has_warned_regression_ = false;
  };
} // namespace wheel_of_fear
