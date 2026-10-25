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
    tree,
  };
} // namespace wheel_of_fear
