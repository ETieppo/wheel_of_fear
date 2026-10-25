#pragma once

#include "core/items/GeneratorPartItem.hpp"

namespace wheel_of_fear {
  class FuseItem final: public GeneratorPartItem {
    public:
      FuseItem()
          : GeneratorPartItem(
              ItemEnum::fuse,
              LocalizedTextProps {
                "FUSE",
                "FUSÍVEL"
              },
              ColorProps::hex(0x18f0ff)
            ) {
      }
  };
} // namespace wheel_of_fear
