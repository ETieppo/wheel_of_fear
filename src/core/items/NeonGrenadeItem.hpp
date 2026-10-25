#pragma once

#include "core/items/ThrowableItem.hpp"
#include "core/theme/Palette.hpp"
#include <algorithm>

namespace wheel_of_fear {
  class NeonGrenadeItem final: public ThrowableItem {
    public:
      NeonGrenadeItem()
          : ThrowableItem(
              ItemEnum::neon_grenade,
              "NEON GRENADE",
              Palette::grenade_neon_colors.front(),
              3,
              0.85F
            ) {
      }

      [[nodiscard]]
      ColorProps colorForPhase(std::uint32_t phase) const override {
        const std::size_t color_index
          = (std::max(phase, 1U) - 1) % Palette::grenade_neon_colors.size();
        return Palette::grenade_neon_colors[color_index];
      }
  };
} // namespace wheel_of_fear
