#pragma once

#include "core/items/GeneratorPartItem.hpp"

namespace wheel_of_fear {
  class ValveItem final: public GeneratorPartItem {
    public:
      ValveItem()
          : GeneratorPartItem(
              ItemEnum::valve,
              LocalizedTextProps {
                "VALVE",
                "VÁLVULA"
              },
              ColorProps::hex(0xff5a3a)
            ) {
      }
  };
} // namespace wheel_of_fear
