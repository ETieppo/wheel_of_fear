#include "mods/player/components/PlayerAimComponent.hpp"

namespace wheel_of_fear {
  void PlayerAimComponent::followMouse(
    Vec2Props holder_position,
    Vec2Props mouse_world_position
  ) {
    const Vec2Props toward_mouse = mouse_world_position.sub(holder_position);

    if(toward_mouse.length() <= minimum_mouse_distance) return;

    direction_ = toward_mouse.normalized();
  }

  void PlayerAimComponent::pointAt(
    Vec2Props direction
  ) {
    direction_ = direction.normalized();
  }
} // namespace wheel_of_fear
