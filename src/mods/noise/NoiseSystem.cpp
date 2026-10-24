#include "mods/noise/NoiseSystem.hpp"
#include "mods/world/TileMapModel.hpp"

namespace wheel_of_fear {
  void NoiseSystem::update(float delta_seconds) {
    std::size_t index = 0;

    while(index < events_.size()) {
      events_[index].remaining_seconds -= delta_seconds;

      if(events_[index].remaining_seconds > 0) {
        index += 1;
        continue;
      }

      events_[index] = events_.back();
      events_.pop_back();
    }
  }

  void NoiseSystem::emit(
    Vec2Props       position,
    float           radius,
    NoiseSourceEnum source
  ) {
    if(events_.size() >= capacity) return;

    events_.push_back(
      NoiseEventProps {
        .position          = position,
        .radius            = radius,
        .source            = source,
        .remaining_seconds = lifetime_seconds,
      }
    );
  }

  std::optional<HeardNoiseDto> NoiseSystem::loudestHeardAt(
    const TileMapModel &map,
    Vec2Props           listener,
    float               hearing_factor
  ) const {
    std::optional<HeardNoiseDto> loudest;

    for(const NoiseEventProps &event : events_) {
      const float distance = event.position.distance(listener);
      float       radius   = event.radius * hearing_factor;

      if(distance > radius) continue;
      if(!map.hasLineOfSight(event.position, listener))
        radius *= occluded_radius_factor;

      const float loudness = 1 - (distance / radius);
      if(loudness <= 0) continue;
      if(!loudest || loudness > loudest->loudness)
        loudest = HeardNoiseDto {.event = event, .loudness = loudness};
    }

    return loudest;
  }
} // namespace wheel_of_fear
