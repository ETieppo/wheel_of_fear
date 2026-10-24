#include "mods/tracker/TrackerSystem.hpp"

#include "core/audio/SoundPlayer.hpp"
#include "core/input/InputController.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/time/GameClock.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/tracker/TrackerConfig.hpp"
#include "mods/tracker/widgets/TrackerRadarWidget.hpp"

namespace wheel_of_fear {
  TrackerSystem::TrackerSystem()
      : last_ping_seconds_(
          TrackerConfig::signal_capacity,
          -10.0F
        ) {
  }

  void TrackerSystem::close() {
    is_open_     = false;
    open_amount_ = 0;
  }

  void TrackerSystem::update(
    GameContextDto &context
  ) {
    if(!context.clock.isRunning()) return;

    toggleOnRequest(context);

    open_amount_ = ScalarMath::approach(
      open_amount_,
      is_open_ ? 1.0F : 0.0F,
      context.clock.deltaSeconds() * opening_speed
    );

    signal_collector_.collect(context, ghost_field_.ghosts());
    ghost_field_.update(context);
    if(is_open_) sweep(context);
  }

  void TrackerSystem::toggleOnRequest(
    GameContextDto &context
  ) {
    if(
      !context.input.wasPressed(InputActionEnum::toggle_tracker) ||
      context.player.isDead()
    )
      return;

    is_open_ = !is_open_;

    context.sound_player.play(
      is_open_ ? SoundEnum::tracker_open : SoundEnum::tracker_close,
      0.7F,
      0,
      1
    );
  }

  void TrackerSystem::sweep(
    GameContextDto &context
  ) {
    const float previous_angle = sweep_angle_;

    sweep_angle_ = ScalarMath::floorModulo(
      sweep_angle_ + (context.clock.deltaSeconds() * sweep_speed),
      ScalarMath::tau
    );

    const Vec2Props player_position = context.player.position();
    const auto     &signals         = signal_collector_.signals();

    for(std::size_t index = 0; index < signals.size(); ++index) {
      const SignalProps &signal = signals[index];
      const Vec2Props    offset = signal.position.sub(player_position);
      if(offset.length() > TrackerConfig::scan_range) continue;

      const float bearing = ScalarMath::floorModulo(
        offset.angle() + ScalarMath::tau,
        ScalarMath::tau
      );
      if(!crossedAngle(previous_angle, sweep_angle_, bearing)) continue;

      last_ping_seconds_[index] = context.clock.realSeconds();

      const float pitch =
        signal.kind == SignalKindEnum::unidentified ? 0.8F : 1.0F;

      context.sound_player.play(
        SoundEnum::tracker_blip,
        0.35F,
        ScalarMath::clamp(offset.x / 300, -1, 1),
        pitch
      );

      context.noise.emit(
        player_position,
        beep_noise_radius,
        NoiseSourceEnum::tracker_beep
      );
    }
  }

  bool TrackerSystem::crossedAngle(
    float previous,
    float current,
    float target
  ) {
    if(current >= previous) return target > previous && target <= current;
    return target > previous || target <= current;
  }

  void TrackerSystem::draw(
    GameContextDto &context
  ) const {
    TrackerRadarWidget::draw(
      context,
      TrackerViewDto {
        .open_amount       = open_amount_,
        .sweep_angle       = sweep_angle_,
        .signals           = signal_collector_.signals(),
        .last_ping_seconds = last_ping_seconds_,
      }
    );
  }
} // namespace wheel_of_fear
