#include "mods/camera/CameraSystem.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/time/GameClock.hpp"
#include "mods/camera/RoomFramer.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>

namespace wheel_of_fear {
  Vec2Props CameraSystem::viewOrigin() const {
    return viewCenter().sub(RoomFramer::half_view);
  }

  Vec2Props
  CameraSystem::mouseWorldPosition(const InputController &input) const {
    return viewOrigin().add(input.mouseInView());
  }

  void CameraSystem::snapToPlayer(
    const WorldSystem  &world,
    const PlayerEntity &player
  ) {
    room_index_         = world.map().roomIndexAt(player.position());
    transition_seconds_ = 0;
    focus_ = framedTarget(room_index_, world, player.position(), Vec2Props {});
    is_following_player_ = true;
  }

  void CameraSystem::glideTo(
    Vec2Props point,
    float     delta_seconds
  ) {
    is_following_player_ = false;
    focus_ = focus_.lerp(point, ScalarMath::clamp(delta_seconds * 1.5F, 0, 1));
  }

  void CameraSystem::update(GameContextDto &context) {
    const WorldSystem &world        = context.world;
    const float        real_seconds = context.clock.realSeconds();
    const float        shake        = world.cameraShake();

    shake_offset_ = Vec2Props::of(
                      ScalarMath::hashTime(real_seconds, 60) - 0.5F,
                      ScalarMath::hashTime(real_seconds + 0.37F, 60) - 0.5F
    )
                      .scale(shake * shake * shake_amplitude);

    if(!is_following_player_ || world.map().width() == 0) return;
    const float         delta  = context.clock.realDeltaSeconds();
    const PlayerEntity &player = context.player;

    if(const auto current_room = world.map().roomIndexAt(player.position())) {
      if(!room_index_ || *room_index_ != *current_room) {
        room_index_         = current_room;
        transition_seconds_ = room_transition_seconds;
      }
    }

    transition_seconds_ = std::max(0.0F, transition_seconds_ - delta);
    const Vec2Props offset
      = player.isHidden() ? Vec2Props {} : mouseOffset(context.input);
    const Vec2Props target
      = framedTarget(room_index_, world, player.position(), offset);
    const float rate
      = transition_seconds_ > 0 ? room_transition_rate : follow_rate;
    focus_ = focus_.lerp(target, ScalarMath::clamp(delta * rate, 0, 1));
  }

  Vec2Props CameraSystem::mouseOffset(const InputController &input) {
    const Vec2Props from_center
      = input.mouseInView().sub(RoomFramer::half_view);

    return Vec2Props::of(
      ScalarMath::clamp(
        from_center.x * mouse_influence.x,
        -maximum_mouse_offset.x,
        maximum_mouse_offset.x
      ),
      ScalarMath::clamp(
        from_center.y * mouse_influence.y,
        -maximum_mouse_offset.y,
        maximum_mouse_offset.y
      )
    );
  }

  Vec2Props CameraSystem::framedTarget(
    std::optional<std::size_t> room_index,
    const WorldSystem         &world,
    Vec2Props                  anchor,
    Vec2Props                  offset
  ) {
    const Vec2Props desired = anchor.add(offset);
    if(!room_index || *room_index >= world.map().rooms().size()) return desired;
    return RoomFramer::clampToRoom(world.map().rooms()[*room_index], desired);
  }
} // namespace wheel_of_fear
