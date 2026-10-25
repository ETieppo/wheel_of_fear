#pragma once

#include "mods/director/DemoModeProps.hpp"
#include "mods/director/demos/DemoScenario.hpp"
#include <memory>

namespace wheel_of_fear {
  class DemoScenarioFactory final {
    public:
      DemoScenarioFactory() = delete;

      static std::unique_ptr<DemoScenario>
      create(const DemoModeProps &demo_mode);

    private:
      static constexpr std::uint32_t deep_sector  = 6;
      static constexpr std::uint32_t first_sector = 1;
  };
} // namespace wheel_of_fear
