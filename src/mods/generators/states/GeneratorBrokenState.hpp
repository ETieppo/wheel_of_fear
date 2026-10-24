#pragma once

#include "mods/generators/states/GeneratorState.hpp"

namespace wheel_of_fear {
  class GeneratorBrokenState final: public GeneratorState {
    public:
      [[nodiscard]]
      ColorProps statusColor(
        const GeneratorEntity &generator
      ) const override;

      [[nodiscard]]
      bool canBeRepaired() const override {
        return true;
      }

      void enter(
        GeneratorEntity &generator,
        GameContextDto  &context
      );
      void update(
        GeneratorEntity          &generator,
        GameContextDto           &context,
        const GeneratorUpdateDto &update
      ) override;

    private:
      static constexpr float wiring_repair_seconds = 2.2F;

      static void emitFaultSparks(
        GeneratorEntity &generator,
        GameContextDto  &context,
        float            delta_seconds
      );
      static void emitFaultLight(
        const GeneratorEntity &generator,
        GameContextDto        &context
      );
      void rewire(
        GeneratorEntity &generator,
        GameContextDto  &context,
        float            delta_seconds
      );

      float wiring_progress_    = 0;
      float repair_sound_timer_ = 0;
  };
} // namespace wheel_of_fear
