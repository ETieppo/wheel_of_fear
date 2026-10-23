#pragma once

#include <cstdint>
#include <span>
#include <string_view>

#include "core/render/Renderer.hpp"

namespace wheel_of_fear {

class PhaseIntroScreen final {
public:
    PhaseIntroScreen() = delete;

    static void draw(Renderer& renderer, std::uint32_t phase, std::span<const std::string_view> introductions, float state_seconds, float intro_seconds, float real_seconds);
};

}
