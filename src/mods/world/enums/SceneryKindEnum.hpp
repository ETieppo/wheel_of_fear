#pragma once

#include <cstdint>

namespace wheel_of_fear {
  enum class SceneryKindEnum : std::uint8_t {
    barrel,
    crate,
    pipes,
    junction_box,
    server_rack,
    terminal,
    neon_sign_arrow,
    neon_sign_eye,
    neon_sign_bars,
  };
} // namespace wheel_of_fear
