#include "core/time/GameClock.hpp"

namespace wheel_of_fear {
  void GameClock::advance(
    float real_delta_seconds
  ) {
    real_delta_seconds_ = real_delta_seconds;
    real_seconds_ += real_delta_seconds;
    delta_seconds_ = is_running_ ? real_delta_seconds : 0;
  }
} // namespace wheel_of_fear
