#include "mods/extra_lives/ExtraLifeSystem.hpp"
#include "mods/world/WorldSystem.hpp"
#include <cstdint>
#include <doctest/doctest.h>
#include <memory>

namespace wheel_of_fear {
  TEST_CASE("extra life never spawns once the maximum health is reached") {
    const auto world = std::make_unique<WorldSystem>();
    world->generatePhase(2, 0x5151);
    ExtraLifeSystem extra_lives;

    for(std::uint64_t seed = 0; seed < 200; ++seed) {
      extra_lives.generatePhase(*world, seed, false);
      CHECK_FALSE(extra_lives.pickup().has_value());
    }
  }

  TEST_CASE("extra life spawns rarely and at most once per phase") {
    const auto world = std::make_unique<WorldSystem>();
    world->generatePhase(2, 0x5151);
    ExtraLifeSystem extra_lives;
    int             spawned_phases = 0;

    for(std::uint64_t seed = 0; seed < 1000; ++seed) {
      extra_lives.generatePhase(*world, seed * 0x9E3779B97F4A7C15ULL, true);
      if(!extra_lives.pickup()) continue;

      spawned_phases += 1;
      CHECK_FALSE(world->map().isSolidAt(extra_lives.pickup()->position()));
    }

    CHECK(spawned_phases > 100);
    CHECK(spawned_phases < 300);
  }
} // namespace wheel_of_fear
