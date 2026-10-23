#pragma once

#include "core/items/GeneratorPartItem.hpp"

namespace wheel_of_fear {

class GearItem final : public GeneratorPartItem {
public:
    GearItem() : GeneratorPartItem(ItemEnum::gear, "GEAR", Color::hex(0xffb020)) {}
};

}
