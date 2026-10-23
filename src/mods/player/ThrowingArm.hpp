#pragma once

#include "core/items/enums/ItemEnum.hpp"
#include "core/math/Vec2.hpp"
#include "game/dtos/GameContextDto.hpp"

namespace wheel_of_fear {

class Inventory;

class ThrowingArm {
public:
    void reset() { cooldown_seconds_ = 0; }
    void tick(float delta_seconds);
    void tryThrow(GameContextDto& context, Inventory& inventory, ItemEnum item, Vec2 from, Vec2 target);

private:
    static constexpr float cooldown_after_throw_seconds = 0.45F;

    float cooldown_seconds_ = 0;
};

}
