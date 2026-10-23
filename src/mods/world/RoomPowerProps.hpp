#pragma once

#include "core/math/Color.hpp"

namespace wheel_of_fear {
  struct RoomPowerProps {
      Color color          = Color::white();
      bool  is_powered     = false;
      float lit_amount     = 0;
      float conduit_offset = 0;

      bool isFullyLit() const {
        return is_powered && lit_amount >= 0.999F;
      }
  };
} // namespace wheel_of_fear
