#pragma once

#include "core/render/Renderer.hpp"
#include "mods/art/SpriteSheetProps.hpp"
#include <cstdint>

namespace wheel_of_fear {
  class InventoryComponent;

  class InventoryBarWidget final {
    public:
      InventoryBarWidget() = delete;

      static void draw(
        Renderer                 &renderer,
        const SpriteSheetProps   &sprites,
        const InventoryComponent &inventory,
        std::uint32_t             phase
      );
  };
} // namespace wheel_of_fear
