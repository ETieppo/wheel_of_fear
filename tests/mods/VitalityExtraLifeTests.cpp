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
} // namespace wheel_of_fear
