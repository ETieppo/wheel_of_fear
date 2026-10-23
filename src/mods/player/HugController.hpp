#pragma once

#include "core/entity/Entity.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/player/enums/PlayerActivityEnum.hpp"

namespace wheel_of_fear {

class HugController {
public:
    PlayerActivityEnum update(GameContextDto& context, Entity& body, Vec2 facing, PlayerActivityEnum current_activity);
};

}
