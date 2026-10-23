#pragma once

#include <cstddef>
#include <optional>

#include "core/math/Vec2.hpp"

namespace wheel_of_fear {

struct MobMemoryProps {
    std::optional<Vec2> last_seen_player;
    float seconds_since_seen = 1000;
    std::optional<Vec2> noise_position;
    float seconds_since_noise = 1000;
    std::optional<std::size_t> known_locker_index;
    float stuck_seconds = 0;
    Vec2 last_progress_position{};
};

}
