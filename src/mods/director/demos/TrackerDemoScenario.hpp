#pragma once

#include "mods/director/demos/DemoScenario.hpp"

namespace wheel_of_fear {
  class TrackerDemoScenario final: public DemoScenario {
    public:
      void run(
        GameContextDto    &context,
        const DemoTickDto &tick
      ) override;

    private:
      static constexpr float open_after_seconds = 1.5F;
  };
} // namespace wheel_of_fear
