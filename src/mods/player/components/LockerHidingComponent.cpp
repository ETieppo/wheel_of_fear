#include "mods/player/components/LockerHidingComponent.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/input/InputController.hpp"
#include "core/time/GameClock.hpp"
#include "mods/hud/HudSystem.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>

namespace wheel_of_fear {
  void LockerHidingComponent::reset() {
    hidden_locker_index_.reset();
    hide_event_locker_index_.reset();
    breath_            = 1;
    is_holding_breath_ = false;
    breathing_timer_   = 0;
  }

  std::optional<std::size_t> LockerHidingComponent::nearestLocker(
    const WorldSystem &world,
    Vec2Props          position
  ) {
    std::optional<std::size_t> best;
    float                      best_distance = LockerEntity::reach;
    const auto                &lockers       = world.lockers();

    for(std::size_t index = 0; index < lockers.size(); ++index) {
      const float distance = lockers[index].entryPoint().distance(position);

      if(distance < best_distance) {
        best_distance = distance;
        best          = index;
      }
    }

    return best;
  }

  void LockerHidingComponent::enter(
    GameContextDto &context,
    Entity         &body,
    std::size_t     locker_index
  ) {
    LockerEntity &locker = context.world.lockers()[locker_index];

    hidden_locker_index_     = locker_index;
    hide_event_locker_index_ = locker_index;
    body.setPosition(locker.position());
    body.setMoving(false);
    breath_ = 1;
    locker.swingDoor();

    context.noise.emit(
      locker.position(),
      locker_noise_radius,
      NoiseSourceEnum::locker_door
    );

    context.sound_player.play(SoundEnum::locker_door, 0.7F, 0, 1);
  }

  void LockerHidingComponent::leave(
    GameContextDto &context,
    Entity         &body
  ) {
    if(!hidden_locker_index_) return;
    LockerEntity &locker = context.world.lockers()[*hidden_locker_index_];

    locker.swingDoor();
    hidden_locker_index_.reset();
    body.setPosition(context.world.map().nearestFloor(locker.entryPoint()));

    context.noise.emit(
      locker.position(),
      locker_noise_radius,
      NoiseSourceEnum::locker_door
    );

    context.sound_player.play(SoundEnum::locker_door, 0.7F, 0, 0.9F);
  }

  void LockerHidingComponent::updateWhileHidden(
    GameContextDto &context,
    Entity         &body
  ) {
    const float delta = context.clock.deltaSeconds();
    context.hud.showPrompt(InputActionEnum::interact, "leave the locker");

    if(context.input.wasPressed(InputActionEnum::interact)) {
      leave(context, body);
      return;
    }

    is_holding_breath_
      = context.input.isHeld(InputActionEnum::hold_breath) && breath_ > 0;

    if(is_holding_breath_) {
      breath_ = std::max(0.0F, breath_ - (delta / breath_hold_seconds));
      if(breath_ > 0) return;

      context.noise
        .emit(body.position(), gasp_noise_radius, NoiseSourceEnum::breathing);
      context.sound_player.play(SoundEnum::breathing, 0.9F, 0, 1.25F);
      breathing_timer_ = breathing_interval_seconds;

      return;
    }

    breath_ = std::min(1.0F, breath_ + (delta / (breath_hold_seconds * 1.5F)));
    breathing_timer_ -= delta;
    if(breathing_timer_ > 0) return;

    breathing_timer_ = breathing_interval_seconds;
    context.noise.emit(
      body.position(),
      breathing_noise_radius,
      NoiseSourceEnum::breathing
    );

    context.sound_player.play(
      SoundEnum::breathing,
      0.3F,
      0,
      context.world.random().range(0.95F, 1.05F)
    );
  }
} // namespace wheel_of_fear
