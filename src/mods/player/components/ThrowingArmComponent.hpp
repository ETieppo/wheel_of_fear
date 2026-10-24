#pragma once

#include "core/items/enums/ItemEnum.hpp"
#include "core/math/Vec2Props.hpp"
#include "game/dtos/GameContextDto.hpp"

namespace wheel_of_fear {
  class InventoryComponent;

  class ThrowingArmComponent {
    public:
      void reset() {
        cooldown_seconds_ = 0;
      }

      void tick(float delta_seconds);

      void tryThrow(
        GameContextDto     &context,
        InventoryComponent &inventory,
        ItemEnum            item,
        Vec2Props           from,
        Vec2Props           target
      );

    private:
      static constexpr float cooldown_after_throw_seconds = 0.45F;
      float                  cooldown_seconds_            = 0;
  };
} // namespace wheel_of_fear
