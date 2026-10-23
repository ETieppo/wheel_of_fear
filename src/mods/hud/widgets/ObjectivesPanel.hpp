#pragma once

#include <cstdint>

#include "core/render/Renderer.hpp"

namespace wheel_of_fear {

class Generators;

class ObjectivesPanel final {
public:
    ObjectivesPanel() = delete;

    static void draw(Renderer& renderer, std::uint32_t phase, const Generators& generators);
};

}
