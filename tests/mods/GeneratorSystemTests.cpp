#include <cstddef>
#include <memory>
#include <vector>

#include <doctest/doctest.h>

#include "support/HeadlessGameHarness.hpp"

namespace wheel_of_fear {
  TEST_CASE(
    "a repaired generator stays online for the rest of the phase"
  ) {
    const auto      harness = std::make_unique<HeadlessGameHarness>("power");
    GameContextDto &context = harness->context();
    const std::vector<GeneratorEntity> &generators = context.generators.all();
    std::vector<bool>                   has_been_online;
    constexpr int                       frames_to_watch = 60 * 60 * 5;

    for(int frame = 0; frame < frames_to_watch; ++frame) {
      harness->advanceFrames(1);

      if(has_been_online.size() != generators.size())
        has_been_online.assign(generators.size(), false);

      for(std::size_t index = 0; index < generators.size(); ++index) {
        if(has_been_online[index]) REQUIRE(generators[index].isOnline());
        if(generators[index].isOnline()) has_been_online[index] = true;
      }
    }

    REQUIRE_FALSE(has_been_online.empty());
    CHECK(has_been_online.front());
  }
} // namespace wheel_of_fear
