#pragma once

#include "mods/world/LedGradientProps.hpp"

namespace wheel_of_fear {
  struct RoomPowerProps {
      LedGradientProps gradient       = {};
      bool             is_powered     = false;
      float            lit_amount     = 0;
      float            conduit_offset = 0;

      [[nodiscard]]
      bool isFullyLit() const {
        return is_powered && lit_amount >= 0.999F;
      }
  };
} // namespace wheel_of_fear
