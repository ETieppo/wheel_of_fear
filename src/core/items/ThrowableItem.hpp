#pragma once

#include "core/items/Item.hpp"

namespace wheel_of_fear {

class ThrowableItem : public Item {
public:
    bool isThrowable() const override { return true; }
    float throwPitch() const { return throw_pitch_; }

protected:
    ThrowableItem(ItemEnum kind, std::string_view display_name, Color color, int carry_limit, float throw_pitch)
        : Item(kind, display_name, color, carry_limit), throw_pitch_(throw_pitch) {}

private:
    float throw_pitch_;
};

}
