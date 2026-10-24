#pragma once

#include <array>

#include "core/math/Color.hpp"

namespace wheel_of_fear {
  class Palette final {
    public:
      Palette() = delete;

      static constexpr Color                neon_cyan    = Color::hex(0x18f0ff);
      static constexpr Color                neon_magenta = Color::hex(0xff2bd6);
      static constexpr Color                neon_violet  = Color::hex(0x8b5cff);
      static constexpr Color                neon_amber   = Color::hex(0xffb020);
      static constexpr Color                neon_lime    = Color::hex(0x7dff4a);
      static constexpr Color                neon_red     = Color::hex(0xff2a4a);
      static constexpr std::array<Color, 5> room_neon_colors {
        neon_cyan,
        neon_magenta,
        neon_violet,
        neon_amber,
        neon_lime
      };

      static constexpr Color flashlight_white  = Color::hex(0xdff6ff);
      static constexpr Color ultraviolet_light = Color::hex(0x9a5cff);
      static constexpr Color warm_light        = Color::hex(0xffd9a0);
      static constexpr Color danger            = Color::hex(0xff3a5a);
      static constexpr Color success           = Color::hex(0x4affc8);
      static constexpr Color text              = Color::hex(0xe2e8f0);
      static constexpr Color muted_text        = Color::hex(0x7c8496);
      static constexpr Color tracker_green     = Color::hex(0x4affb4);
      static constexpr Color brass             = Color::hex(0xffb020);
      static constexpr Color spark_yellow      = Color::hex(0xffe060);
      static constexpr Color spark_blue        = Color::hex(0x7ad8ff);
      static constexpr Color panel             = Color::hex(0x05060c);
      static constexpr Color panel_deep        = Color::hex(0x070a14);
      static constexpr Color panel_edge        = Color::hex(0x1fd8ff);
      static constexpr Color hud_magenta       = Color::hex(0xff3fd0);
      static constexpr Color ink               = Color::hex(0x0b0e18);
  };
} // namespace wheel_of_fear
