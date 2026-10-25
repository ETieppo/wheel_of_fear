#pragma once

#include "core/render/Renderer.hpp"
#include <cstdint>
#include <optional>

namespace wheel_of_fear {
  class TitleScreen final {
    public:
      TitleScreen() = delete;

      static void draw(
        Renderer                    &renderer,
        float                        real_seconds,
        std::optional<std::uint32_t> saved_phase,
        bool                         is_credits_hovered
      );
  };
} // namespace wheel_of_fear
