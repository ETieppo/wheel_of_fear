#include <cmath>

#include <doctest/doctest.h>

#include "core/entity/Entity.hpp"

namespace wheel_of_fear {
  TEST_CASE(
    "turning converges on the target angle through the shortest arc"
  ) {
    Entity entity(Vec2Props::of(0, 0), 4);

    entity.setHeading(3.0F);

    for(int step = 0; step < 120; ++step)
      entity.turnToward(-3.0F, 1.0F / 60.0F, 9);

    CHECK(std::abs(std::sin(entity.heading()) - std::sin(-3.0F)) < 0.01F);
    CHECK(std::abs(std::cos(entity.heading()) - std::cos(-3.0F)) < 0.01F);
  }
} // namespace wheel_of_fear
