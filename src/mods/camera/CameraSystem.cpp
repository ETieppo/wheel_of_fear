#include "mods/camera/CameraSystem.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/time/GameClock.hpp"
#include "mods/camera/RoomFramer.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"

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
    room_index_          = world.map().roomIndexAt(player.position());
    focus_               = player.position();
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

    const PlayerEntity &player = context.player;

    if(const auto current_room = world.map().roomIndexAt(player.position()))
      room_index_ = current_room;

    focus_ = player.position();
  }
} // namespace wheel_of_fear
