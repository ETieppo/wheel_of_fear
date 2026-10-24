#pragma once

#include "core/render/Renderer.hpp"
#include <cstdint>
#include <span>
#include <string_view>

namespace wheel_of_fear {
  class PhaseIntroScreen final {
    public:
      PhaseIntroScreen() = delete;

      static void draw(
        Renderer                         &renderer,
        std::uint32_t                     phase,
        std::span<const std::string_view> introductions,
        float                             state_seconds,
        float                             intro_seconds,
        float                             real_seconds
      );
  };
} // namespace wheel_of_fear
