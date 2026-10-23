#pragma once

#include <array>

namespace wheel_of_fear {

struct QuadViewProps {
    std::array<float, 2> origin{};
    std::array<float, 2> size{};
    std::array<float, 2> atlas_size{};
    float time = 0;
    float padding = 0;
};

static_assert(sizeof(QuadViewProps) == 32);

}
