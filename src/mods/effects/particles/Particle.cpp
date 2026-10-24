#include "mods/effects/particles/Particle.hpp"
#include <algorithm>

namespace wheel_of_fear {
  bool Particle::advance(float delta_seconds) {
    state_.remaining_seconds -= delta_seconds;
    if(state_.remaining_seconds <= 0) return false;

    state_.position = state_.position.add(state_.velocity.scale(delta_seconds));
    state_.rotation += state_.spin * delta_seconds;
    state_.velocity
      = state_.velocity.scale(std::max(0.0F, 1 - (delta_seconds * drag())));

    if(fallsWithGravity()) state_.velocity.y += gravity * delta_seconds;
    return true;
  }
} // namespace wheel_of_fear
