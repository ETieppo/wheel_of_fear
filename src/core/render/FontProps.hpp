#pragma once

#include <array>

#include "core/render/GlyphProps.hpp"

namespace wheel_of_fear {

struct FontProps {
    std::array<GlyphProps, 256> glyphs{};
    float pixel_size = 64;
};

}
