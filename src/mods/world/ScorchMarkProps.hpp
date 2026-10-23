#pragma once

#include <cstddef>

#include "core/math/Vec2.hpp"

namespace wheel_of_fear {

struct ScorchMarkProps {
    Vec2 position{};
    float rotation = 0;
    std::size_t variant = 0;
};

}
