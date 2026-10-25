#pragma once

#include "mods/director/demos/DemoScenario.hpp"

namespace wheel_of_fear {
  class EndingDemoScenario final: public DemoScenario {
    public:
      [[nodiscard]]
      bool startsAtEnding() const override {
        return true;
      }

      void run(
        GameContextDto    &context,
        const DemoTickDto &tick
      ) override;

    private:
      static constexpr float walk_seconds = 4;
  };
} // namespace wheel_of_fear
