#pragma once

#include "core/items/GeneratorPartItem.hpp"

namespace wheel_of_fear {

class ValveItem final : public GeneratorPartItem {
public:
    ValveItem() : GeneratorPartItem(ItemEnum::valve, "VALVE", Color::hex(0xff5a3a)) {}
};

}
