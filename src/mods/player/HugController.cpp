#include "mods/player/HugController.hpp"

#include "core/input/Input.hpp"
#include "mods/mobs/Mobs.hpp"

namespace wheel_of_fear {
  PlayerActivityEnum HugController::update(
    GameContextDto    &context,
    Entity            &body,
    Vec2               facing,
    PlayerActivityEnum current_activity
  ) {
    if(!context.input.isHeld(InputActionEnum::hug)) {
      if(current_activity == PlayerActivityEnum::hugging)
        context.mobs.cancelHug();
      return PlayerActivityEnum::exploring;
    }
    if(context.input.wasPressed(InputActionEnum::hug))
      context.mobs.tryStartHug(body.position(), facing);
    if(!context.mobs.isHugInProgress()) return PlayerActivityEnum::exploring;
    body.setMoving(false);
    return PlayerActivityEnum::hugging;
  }
} // namespace wheel_of_fear
