#pragma once

#include "mods/director/demos/DemoScenario.hpp"

namespace wheel_of_fear {
  class LureDemoScenario final: public DemoScenario {
    public:
      void run(
        GameContextDto    &context,
        const DemoTickDto &tick
      ) override;
  };
} // namespace wheel_of_fear
