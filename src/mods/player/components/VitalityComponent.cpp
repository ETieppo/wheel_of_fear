#include "mods/player/components/VitalityComponent.hpp"

#include <algorithm>

namespace wheel_of_fear {
  void VitalityComponent::restore() {
    health_               = max_health;
    invulnerable_seconds_ = spawn_invulnerability_seconds;
    hurt_flash_           = 0;
    is_dead_              = false;
  }

  void VitalityComponent::tick(
    float delta_seconds
  ) {
    invulnerable_seconds_ =
      std::max(0.0F, invulnerable_seconds_ - delta_seconds);

    hurt_flash_ = std::max(0.0F, hurt_flash_ - (delta_seconds * 2));
  }

  bool VitalityComponent::absorbHit() {
    if(invulnerable_seconds_ > 0 || is_dead_) return false;

    health_ -= 1;
    invulnerable_seconds_ = hit_invulnerability_seconds;
    hurt_flash_           = 1;
    if(health_ <= 0) is_dead_ = true;

    return true;
  }
} // namespace wheel_of_fear
