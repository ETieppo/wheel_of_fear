#include "mods/flashlight/FlashlightBattery.hpp"

#include <algorithm>

namespace wheel_of_fear {
  void FlashlightBattery::refill() {
    level_         = 1;
    is_charging_   = false;
    flicker_timer_ = 0;
  }

  void FlashlightBattery::drain(
    float delta_seconds,
    float seconds_to_drain
  ) {
    level_ = std::max(0.0F, level_ - delta_seconds / seconds_to_drain);
  }

  bool FlashlightBattery::updateCharging(
    bool  can_charge,
    float delta_seconds
  ) {
    const bool was_charging = is_charging_;
    is_charging_            = level_ < 1 && can_charge;
    if(is_charging_)
      level_ = std::min(1.0F, level_ + delta_seconds / recharge_seconds);
    return is_charging_ && !was_charging;
  }

  bool FlashlightBattery::flickersOff(
    RandomGenerator &random,
    float            delta_seconds
  ) {
    flicker_timer_ -= delta_seconds;
    if(level_ <= 0 || level_ >= low_threshold) return false;
    if(flicker_timer_ <= 0)
      flicker_timer_ =
        random.range(0.05F, 0.8F * level_ / low_threshold + 0.1F);
    return flicker_timer_ < 0.06F;
  }
} // namespace wheel_of_fear
