#pragma once

#include "core/math/Vec2Props.hpp"
#include "core/render/RenderConfig.hpp"
#include <cstddef>

namespace wheel_of_fear {
  class TrackerConfig final {
    public:
      TrackerConfig() = delete;

      static constexpr std::size_t signal_capacity = 48;
      static constexpr std::size_t ghost_capacity  = 6;
      static constexpr float       scan_range      = 760;
      static constexpr float       radar_radius    = 215;
      static constexpr Vec2Props   radar_center {
        .x = RenderConfig::ui_width / 2,
        .y = (RenderConfig::ui_height / 2) + 14,
      };
  };
} // namespace wheel_of_fear
