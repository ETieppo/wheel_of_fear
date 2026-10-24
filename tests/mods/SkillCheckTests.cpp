#include <doctest/doctest.h>

#include "mods/generators/SkillCheck.hpp"

namespace wheel_of_fear {
  TEST_CASE(
    "the skill check judges the needle against its zones"
  ) {
    SkillCheck check(2, 1, 0.2F);
    check.setNeedleAngle(1.5F);
    CHECK(check.judge() == SkillCheckResultEnum::failed);
    check.setNeedleAngle(2.1F);
    CHECK(check.judge() == SkillCheckResultEnum::good);
    check.setNeedleAngle(2.5F);
    CHECK(check.judge() == SkillCheckResultEnum::perfect);
    check.setNeedleAngle(3.2F);
    CHECK(check.hasMissed());
  }
} // namespace wheel_of_fear
