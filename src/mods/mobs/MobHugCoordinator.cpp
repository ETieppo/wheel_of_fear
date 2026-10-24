#include "mods/mobs/MobHugCoordinator.hpp"
#include <algorithm>

namespace wheel_of_fear {
  bool MobHugCoordinator::isHugInProgress() const {
    return std::ranges::any_of(mobs_, [](const auto &mob) {
      return mob->condition().isBeingHugged();
    });
  }

  bool MobHugCoordinator::tryStartHug(
    Vec2Props position,
    Vec2Props facing
  ) {
    MobEntity* closest          = nullptr;
    float      closest_distance = hug_reach;

    for(const auto &mob : mobs_) {
      if(mob->isBusy() || !mob->traits().is_huggable || mob->isDisguised())
        continue;

      const Vec2Props offset   = mob->position().sub(position);
      const float     distance = offset.length();
      if(
        distance >= closest_distance || offset.normalized().dot(facing) < -0.3F
      )
        continue;

      closest_distance = distance;
      closest          = mob.get();
    }

    if(closest == nullptr) return false;
    closest->condition().setBeingHugged(true);

    return true;
  }

  void MobHugCoordinator::cancelHug() {
    for(const auto &mob : mobs_) mob->condition().setBeingHugged(false);
  }
} // namespace wheel_of_fear
