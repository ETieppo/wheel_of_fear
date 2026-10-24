#pragma once

#include <cstddef>

#include "mods/director/demos/DemoScenario.hpp"

namespace wheel_of_fear {
  class DoorsDemoScenario final: public DemoScenario {
    public:
      void run(
        GameContextDto    &context,
        const DemoTickDto &tick
      ) override;

    private:
      std::size_t total_moved_doorways_ = 0;
  };
} // namespace wheel_of_fear
