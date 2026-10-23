#pragma once

#include "core/items/GeneratorPartItem.hpp"

namespace wheel_of_fear {
  class FuseItem final: public GeneratorPartItem {
    public:
      FuseItem()
          : GeneratorPartItem(
              ItemEnum::fuse,
              "FUSE",
              Color::hex(0x18f0ff)
            ) {
      }
  };
} // namespace wheel_of_fear
