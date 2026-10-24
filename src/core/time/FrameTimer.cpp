#include "core/time/FrameTimer.hpp"

namespace wheel_of_fear {
  FrameTimer::FrameTimer()
      : last_lap_(std::chrono::steady_clock::now()) {
  }

  float FrameTimer::lap() {
    const auto                         now = std::chrono::steady_clock::now();
    const std::chrono::duration<float> elapsed = now - last_lap_;

    last_lap_ = now;

    return elapsed.count();
  }
} // namespace wheel_of_fear
