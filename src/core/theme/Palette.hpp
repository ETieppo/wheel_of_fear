#pragma once

#include "core/math/ColorProps.hpp"
#include <array>

namespace wheel_of_fear {
  class Palette final {
    public:
      Palette() = delete;

      static constexpr ColorProps neon_cyan    = ColorProps::hex(0x18f0ff);
      static constexpr ColorProps neon_magenta = ColorProps::hex(0xff2bd6);
      static constexpr ColorProps neon_violet  = ColorProps::hex(0x8b5cff);
      static constexpr ColorProps neon_amber   = ColorProps::hex(0xffb020);
      static constexpr ColorProps neon_lime    = ColorProps::hex(0x7dff4a);
      static constexpr ColorProps neon_red     = ColorProps::hex(0xff2a4a);
      static constexpr std::array<ColorProps, 5> room_neon_colors {
        neon_cyan,
        neon_magenta,
        neon_violet,
        neon_amber,
        neon_lime,
      };
      static constexpr ColorProps flashlight_white  = ColorProps::hex(0xdff6ff);
      static constexpr ColorProps ultraviolet_light = ColorProps::hex(0x9a5cff);
      static constexpr ColorProps warm_light        = ColorProps::hex(0xffd9a0);
      static constexpr ColorProps danger            = ColorProps::hex(0xff3a5a);
      static constexpr ColorProps success           = ColorProps::hex(0x4affc8);
      static constexpr ColorProps text              = ColorProps::hex(0xe2e8f0);
      static constexpr ColorProps muted_text        = ColorProps::hex(0x7c8496);
      static constexpr ColorProps tracker_green     = ColorProps::hex(0x4affb4);
      static constexpr ColorProps brass             = ColorProps::hex(0xffb020);
      static constexpr ColorProps spark_yellow      = ColorProps::hex(0xffe060);
      static constexpr ColorProps spark_blue        = ColorProps::hex(0x7ad8ff);
      static constexpr ColorProps panel             = ColorProps::hex(0x05060c);
      static constexpr ColorProps panel_deep        = ColorProps::hex(0x070a14);
      static constexpr ColorProps panel_edge        = ColorProps::hex(0x1fd8ff);
      static constexpr ColorProps accent            = neon_amber;
      static constexpr ColorProps ink               = ColorProps::hex(0x0b0e18);
  };
} // namespace wheel_of_fear
