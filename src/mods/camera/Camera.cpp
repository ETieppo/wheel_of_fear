#include "mods/camera/Camera.hpp"

#include <algorithm>

#include "core/math/Scalar.hpp"
#include "core/time/GameClock.hpp"
#include "mods/camera/RoomFraming.hpp"
#include "mods/player/Player.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  Vec2 Camera::viewOrigin() const {
    return viewCenter().sub(RoomFraming::half_view);
  }

  Vec2 Camera::mouseWorldPosition(const Input &input) const {
    return viewOrigin().add(input.mouseInView());
  }

  void Camera::snapToPlayer(
    const World  &world,
    const Player &player
  ) {
    room_index_         = world.map().roomIndexAt(player.position());
    transition_seconds_ = 0;
    focus_ = framedTarget(room_index_, world, player.position(), Vec2 {});
    is_following_player_ = true;
  }

  void Camera::glideTo(
    Vec2  point,
    float delta_seconds
  ) {
    is_following_player_ = false;
    focus_ = focus_.lerp(point, Scalar::clamp(delta_seconds * 1.5F, 0, 1));
  }

  void Camera::update(GameContextDto &context) {
    const World &world        = context.world;
    const float  real_seconds = context.clock.realSeconds();
    const float  shake        = world.cameraShake();
    shake_offset_             = Vec2::of(
                                  Scalar::hashTime(real_seconds, 60) - 0.5F,
                                  Scalar::hashTime(real_seconds + 0.37F, 60) - 0.5F
    )
                                  .scale(shake * shake * shake_amplitude);
    if(!is_following_player_ || world.map().width() == 0) return;
    const float   delta  = context.clock.realDeltaSeconds();
    const Player &player = context.player;
    if(const auto current_room = world.map().roomIndexAt(player.position())) {
      if(!room_index_ || *room_index_ != *current_room) {
        room_index_         = current_room;
        transition_seconds_ = room_transition_seconds;
      }
    }
    transition_seconds_ = std::max(0.0F, transition_seconds_ - delta);
    const Vec2 offset =
      player.isHidden() ? Vec2 {} : mouseOffset(context.input);
    const Vec2 target =
      framedTarget(room_index_, world, player.position(), offset);
    const float rate =
      transition_seconds_ > 0 ? room_transition_rate : follow_rate;
    focus_ = focus_.lerp(target, Scalar::clamp(delta * rate, 0, 1));
  }

  Vec2 Camera::mouseOffset(const Input &input) {
    const Vec2 from_center = input.mouseInView().sub(RoomFraming::half_view);
    return Vec2::of(
      Scalar::clamp(
        from_center.x * mouse_influence.x,
        -maximum_mouse_offset.x,
        maximum_mouse_offset.x
      ),
      Scalar::clamp(
        from_center.y * mouse_influence.y,
        -maximum_mouse_offset.y,
        maximum_mouse_offset.y
      )
    );
  }

  Vec2 Camera::framedTarget(
    std::optional<std::size_t> room_index,
    const World               &world,
    Vec2                       anchor,
    Vec2                       offset
  ) {
    const Vec2 desired = anchor.add(offset);
    if(!room_index || *room_index >= world.map().rooms().size()) return desired;
    return RoomFraming::clampToRoom(world.map().rooms()[*room_index], desired);
  }
} // namespace wheel_of_fear
