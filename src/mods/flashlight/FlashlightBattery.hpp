#pragma once

#include "core/math/RandomGenerator.hpp"

namespace wheel_of_fear {

class FlashlightBattery {
public:
    static constexpr float low_threshold = 0.18F;

    float level() const { return level_; }
    bool isCharging() const { return is_charging_; }
    bool isEmpty() const { return level_ <= 0; }

    void refill();
    void drain(float delta_seconds, float seconds_to_drain);
    bool updateCharging(bool can_charge, float delta_seconds);
    bool flickersOff(RandomGenerator& random, float delta_seconds);

private:
    static constexpr float recharge_seconds = 6;

    float level_ = 1;
    bool is_charging_ = false;
    float flicker_timer_ = 0;
};

}
