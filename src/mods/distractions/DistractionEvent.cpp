#include "mods/distractions/DistractionEvent.hpp"
#include "core/math/ScalarMath.hpp"
#include "mods/distractions/DistractionCatalog.hpp"
#include <algorithm>

namespace wheel_of_fear {
  DistractionEvent::DistractionEvent(
    DistractionKindEnum kind,
    Vec2Props           position
  )
      : traits_(DistractionCatalog::traitsOf(kind))
      , position_(position) {
  }

  float DistractionEvent::flashStrength() const {
    if(!traits_.flash_color || elapsed_seconds_ > traits_.flash_seconds)
      return 0;

    const float progress = elapsed_seconds_ / traits_.flash_seconds;
    const float stutter
      = ScalarMath::floorModulo(elapsed_seconds_ * 30, 1) < 0.5F ? 1.0F : 0.4F;

    return (1 - progress) * stutter;
  }

  bool DistractionEvent::isFinished() const {
    return elapsed_seconds_ > std::max(traits_.flash_seconds, 0.1F);
  }

  void DistractionEvent::emitFlash(LightingSystem &lighting) const {
    const float strength = flashStrength();
    if(strength <= 0 || !traits_.flash_color) return;

    LightProps flash;
    flash.position  = position_;
    flash.radius    = traits_.flash_radius;
    flash.intensity = 1.8F * strength;
    flash.color     = *traits_.flash_color;
    lighting.add(flash);
  }
} // namespace wheel_of_fear
