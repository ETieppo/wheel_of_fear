#pragma once

#include "core/render/Renderer.hpp"
#include <string_view>
#include <vector>

namespace wheel_of_fear {
  class UiTextWrapper final {
    public:
      UiTextWrapper() = delete;

      static std::vector<std::string_view> wrap(
        const Renderer  &renderer,
        std::string_view text,
        float            size,
        float            maximum_width
      );
  };
} // namespace wheel_of_fear
