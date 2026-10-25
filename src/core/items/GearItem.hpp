#pragma once

#include "core/items/GeneratorPartItem.hpp"

namespace wheel_of_fear {
  class GearItem final: public GeneratorPartItem {
    public:
      GearItem()
          : GeneratorPartItem(
              ItemEnum::gear,
              LocalizedTextProps {
                "GEAR",
                "ENGRENAGEM"
              },
              ColorProps::hex(0xffb020)
            ) {
      }
  };
} // namespace wheel_of_fear
