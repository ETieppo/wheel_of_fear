#pragma once

#include <cstdint>

#include "mods/director/demos/DemoScenario.hpp"

namespace wheel_of_fear {
  class SectorsDemoScenario final: public DemoScenario {
    public:
      void run(
        GameContextDto    &context,
        const DemoTickDto &tick
      ) override;

    private:
      std::uint32_t last_phase_ = 0;
  };
} // namespace wheel_of_fear
