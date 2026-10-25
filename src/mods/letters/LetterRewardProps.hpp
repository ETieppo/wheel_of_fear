#pragma once

#include "core/items/enums/ItemEnum.hpp"

namespace wheel_of_fear {
  struct LetterRewardProps {
      ItemEnum item  = ItemEnum::neon_grenade;
      int      count = 1;
  };
} // namespace wheel_of_fear
