#pragma once

#include <string_view>

#include "core/math/Color.hpp"
#include "core/render/Renderer.hpp"

namespace wheel_of_fear {

class CenteredCard final {
public:
    CenteredCard() = delete;

    static void draw(Renderer& renderer, std::string_view title, std::string_view subtitle, Color color, float real_seconds);
};

}
