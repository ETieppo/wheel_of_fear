#include "mods/player/components/VitalityComponent.hpp"
#include <doctest/doctest.h>

namespace wheel_of_fear {
  TEST_CASE("extra life raises both health and its maximum") {
    VitalityComponent vitality;

    CHECK(vitality.grantExtraLife());
    CHECK(vitality.maxHealth() == VitalityComponent::base_max_health + 1);
    CHECK(vitality.health() == VitalityComponent::base_max_health + 1);
  }

  TEST_CASE("extra lives stop at the highest maximum health") {
    VitalityComponent vitality;

    while(vitality.grantExtraLife()) {}

    CHECK(vitality.maxHealth() == VitalityComponent::highest_max_health);
    CHECK_FALSE(vitality.grantExtraLife());
  }

  TEST_CASE("restoring vitality refills up to the raised maximum") {
    VitalityComponent vitality;
    vitality.setMaxHealth(6);
    vitality.restore();

    CHECK(vitality.health() == 6);
  }

  TEST_CASE("maximum health is clamped to the allowed range") {
    VitalityComponent vitality;

    vitality.setMaxHealth(20);
    CHECK(vitality.maxHealth() == VitalityComponent::highest_max_health);

    vitality.setMaxHealth(0);
    CHECK(vitality.maxHealth() == VitalityComponent::base_max_health);
  }

  TEST_CASE("god mode ignores every hit") {
    VitalityComponent vitality;
    vitality.setGodMode(true);

    for(int hit = 0; hit < 10; ++hit) {
      vitality.dropGuard();
      CHECK_FALSE(vitality.absorbHit());
    }

    CHECK(vitality.health() == VitalityComponent::base_max_health);
    CHECK_FALSE(vitality.isDead());
  }

  TEST_CASE("god mode survives a respawn") {
    VitalityComponent vitality;
    vitality.setGodMode(true);
    vitality.restore();
    vitality.dropGuard();

    CHECK(vitality.isGodMode());
    CHECK_FALSE(vitality.absorbHit());
  }

  TEST_CASE("without god mode an unguarded hit costs health") {
    VitalityComponent vitality;
    vitality.dropGuard();

    CHECK(vitality.absorbHit());
    CHECK(vitality.health() == VitalityComponent::base_max_health - 1);
  }
} // namespace wheel_of_fear
