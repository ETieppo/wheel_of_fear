#pragma once

#include <chrono>

namespace wheel_of_fear {

class FrameTimer {
public:
    FrameTimer();

    float lap();

private:
    std::chrono::steady_clock::time_point last_lap_;
};

}
