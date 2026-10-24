#pragma once

#include "core/entity/Entity.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/player/enums/PlayerActivityEnum.hpp"

namespace wheel_of_fear {
  class HugController {
    public:
      static PlayerActivityEnum update(
        GameContextDto    &context,
        Entity            &body,
        Vec2Props          facing,
        PlayerActivityEnum current_activity
      );
  };
} // namespace wheel_of_fear
