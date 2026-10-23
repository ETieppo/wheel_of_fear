#pragma once

#include <cstdint>
#include <vector>

namespace wheel_of_fear {

struct DecodedImageProps {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::vector<std::uint8_t> pixels;
};

}
