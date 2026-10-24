#pragma once

#include <memory>

#include "mods/director/demos/DemoScenario.hpp"
#include "mods/director/enums/DemoModeEnum.hpp"

namespace wheel_of_fear {
  class DemoScenarioFactory final {
    public:
      DemoScenarioFactory() = delete;

      static std::unique_ptr<DemoScenario> create(
        DemoModeEnum mode
      );

    private:
      static constexpr std::uint32_t deep_sector = 6;
  };
} // namespace wheel_of_fear
