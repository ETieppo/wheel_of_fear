#pragma once

#include "core/items/ThrowableItem.hpp"

namespace wheel_of_fear {

class NeonGrenadeItem final : public ThrowableItem {
public:
    NeonGrenadeItem() : ThrowableItem(ItemEnum::neon_grenade, "NEON GRENADE", Color::hex(0xff2bd6), 3, 0.85F) {}
};

}
