#pragma once

#include "mods/director/demos/DemoScenario.hpp"

namespace wheel_of_fear {
  class LockerDemoScenario final: public DemoScenario {
    public:
      explicit LockerDemoScenario(bool has_witness)
          : has_witness_(has_witness) {
      }

      void run(
        GameContextDto    &context,
        const DemoTickDto &tick
      ) override;

    private:
      bool has_witness_;
  };
} // namespace wheel_of_fear
