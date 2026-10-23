#pragma once

#include "core/render/Renderer.hpp"

namespace wheel_of_fear {

class LockerHiding;

class HidingOverlay final {
public:
    HidingOverlay() = delete;

    static void draw(Renderer& renderer, const LockerHiding& hiding, float real_seconds);
};

}
