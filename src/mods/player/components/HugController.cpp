#include "mods/player/components/HugController.hpp"
#include "core/input/InputController.hpp"
#include "mods/mobs/MobSystem.hpp"

namespace wheel_of_fear {
  PlayerActivityEnum HugController::update(
    GameContextDto    &context,
    Entity            &body,
    Vec2Props          facing,
    PlayerActivityEnum current_activity
  ) {
    if(!context.input.isHeld(InputActionEnum::hug)) {
      if(current_activity == PlayerActivityEnum::hugging)
        context.mobs.hugs().cancelHug();
      return PlayerActivityEnum::exploring;
    }

    if(context.input.wasPressed(InputActionEnum::hug))
      context.mobs.hugs().tryStartHug(body.position(), facing);
    if(!context.mobs.hugs().isHugInProgress())
      return PlayerActivityEnum::exploring;

    body.setMoving(false);
    return PlayerActivityEnum::hugging;
  }
} // namespace wheel_of_fear
