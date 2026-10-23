#pragma once

#include "core/items/ThrowableItem.hpp"

namespace wheel_of_fear {
  class LureBeaconItem final: public ThrowableItem {
    public:
      LureBeaconItem()
          : ThrowableItem(
              ItemEnum::lure_beacon,
              "LURE BEACON",
              Color::hex(0x7dff4a),
              4,
              1.1F
            ) {
      }
  };
} // namespace wheel_of_fear
