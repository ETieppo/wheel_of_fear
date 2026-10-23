#pragma once

#include "core/render/Renderer.hpp"

namespace wheel_of_fear {

class Player;

class VitalsPanel final {
public:
    VitalsPanel() = delete;

    static void draw(Renderer& renderer, const Player& player, float real_seconds);
};

}
