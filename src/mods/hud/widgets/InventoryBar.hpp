#pragma once

#include "core/render/Renderer.hpp"
#include "mods/art/SpriteSheetProps.hpp"

namespace wheel_of_fear {

class Inventory;

class InventoryBar final {
public:
    InventoryBar() = delete;

    static void draw(Renderer& renderer, const SpriteSheetProps& sprites, const Inventory& inventory);
};

}
