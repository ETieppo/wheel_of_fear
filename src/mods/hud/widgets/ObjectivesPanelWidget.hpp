#pragma once

#include "core/render/Renderer.hpp"
#include <cstdint>

namespace wheel_of_fear {
  class GeneratorSystem;

  class ObjectivesPanelWidget final {
    public:
      ObjectivesPanelWidget() = delete;

      static void draw(
        Renderer              &renderer,
        std::uint32_t          phase,
        const GeneratorSystem &generators
      );
  };
} // namespace wheel_of_fear
