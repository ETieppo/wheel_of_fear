#include "mods/player/PlayerMovement.hpp"

#include <algorithm>

#include "core/audio/SoundPlayer.hpp"
#include "core/input/Input.hpp"
#include "core/time/GameClock.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/tracker/Tracker.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  void PlayerMovement::reset() {
    knockback_       = Vec2 {};
    stride_distance_ = 0;
    footstep_timer_  = 0;
    is_running_      = false;
  }

  void PlayerMovement::update(
    GameContextDto &context,
    Entity         &body
  ) {
    const float delta     = context.clock.deltaSeconds();
    Vec2        direction = context.input.movementDirection();
    if(context.tracker.isOpen()) direction = direction.scale(tracker_slowdown);
    is_running_ =
      context.input.isHeld(InputActionEnum::run) && direction.length() > 0.1F;

    float speed = is_running_ ? run_speed : walk_speed;
    if(context.world.map().acidAt(body.position()) > 0) speed *= 0.5F;
    const Vec2 before = body.position();
    body.slide(
      context.world.map(),
      direction.scale(speed * delta).add(knockback_.scale(delta))
    );
    knockback_            = knockback_.scale(std::max(0.0F, 1 - delta * 8));
    const float travelled = body.position().distance(before);
    body.setMoving(direction.length() > 0.1F && travelled > 0.01F);
    if(body.isMoving()) stepFootsteps(context, body, travelled);
  }

  void PlayerMovement::stepFootsteps(
    GameContextDto &context,
    const Entity   &body,
    float           travelled
  ) {
    stride_distance_ += travelled;
    footstep_timer_ -= context.clock.deltaSeconds();
    if(footstep_timer_ > 0) return;
    footstep_timer_ = is_running_ ? 0.27F : 0.42F;
    context.sound_player.play(
      SoundEnum::footstep,
      is_running_ ? 0.5F : 0.22F,
      0,
      context.world.random().range(0.9F, 1.15F)
    );
    context.noise.emit(
      body.position(),
      is_running_ ? running_noise_radius : walking_noise_radius,
      is_running_ ? NoiseSourceEnum::running : NoiseSourceEnum::footstep
    );
  }
} // namespace wheel_of_fear
