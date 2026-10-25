#include "mods/mobs/MobCatalog.hpp"
#include "mods/mobs/components/MobLocomotionComponent.hpp"
#include "mods/mobs/species/MimicMob.hpp"
#include "mods/player/components/PlayerMovementComponent.hpp"
#include <doctest/doctest.h>

namespace wheel_of_fear {
  TEST_CASE(
    "a running player always outpaces every hostile mob, even frenzied"
  ) {
    for(const MobKindEnum kind : all_mob_kinds) {
      const MobTraitsProps &traits = MobCatalog::traitsOf(kind);
      if(!traits.is_hostile) continue;

      CAPTURE(traits.display_name.english);
      CHECK(
        traits.speed
        * MobLocomotionComponent::frenzy_speed_multiplier
        < PlayerMovementComponent::run_speed
      );
    }
  }

  TEST_CASE("the mimic lunge is a burst the running player can still escape") {
    CHECK(MimicMob::lunge_speed < PlayerMovementComponent::run_speed);
  }
} // namespace wheel_of_fear
