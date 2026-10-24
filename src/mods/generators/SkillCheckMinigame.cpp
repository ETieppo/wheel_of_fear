#include "mods/generators/SkillCheckMinigame.hpp"

#include <algorithm>

#include "core/math/ScalarMath.hpp"

namespace wheel_of_fear {
  SkillCheckMinigame::SkillCheckMinigame(
    const SkillCheckZoneProps &zone
  )
      : zone_start_(zone.zone_start)
      , zone_width_(zone.zone_width)
      , perfect_width_(zone.perfect_width) {
  }

  SkillCheckResultEnum SkillCheckMinigame::judge() const {
    if(needle_angle_ < zone_start_ || needle_angle_ > zoneEnd())
      return SkillCheckResultEnum::failed;

    const float perfect_start = perfectStart();

    if(
      needle_angle_ >=
      perfect_start &&
      needle_angle_ <=
      perfect_start +
      perfect_width_
    )
      return SkillCheckResultEnum::perfect;

    return SkillCheckResultEnum::good;
  }

  bool SkillCheckMinigame::hasMissed() const {
    return needle_angle_ >
           zoneEnd() +
           0.05F ||
           needle_angle_ >= ScalarMath::tau;
  }

  void SkillCheckMinigame::tickResult(
    float delta_seconds
  ) {
    result_seconds_ = std::max(0.0F, result_seconds_ - delta_seconds);
  }

  bool SkillCheckMinigame::tickCountdown(
    float delta_seconds
  ) {
    countdown_seconds_ -= delta_seconds;
    return countdown_seconds_ <= 0;
  }

  void SkillCheckMinigame::begin(
    std::uint32_t    phase,
    RandomGenerator &random
  ) {
    const auto difficulty = static_cast<float>(std::min(phase, 6U));

    is_active_              = true;
    warning_seconds_        = warning_duration_seconds;
    needle_angle_           = 0;
    zone_start_             = random.range(1.7F, 5.0F);
    zone_width_             = std::max(0.5F, 1.0F - (difficulty * 0.07F));
    perfect_width_          = 0.22F;
    revolutions_per_second_ = 0.75F + (difficulty * 0.06F);
    countdown_seconds_      = 2.5F;
  }

  std::optional<SkillCheckResultEnum> SkillCheckMinigame::advanceNeedle(
    float delta_seconds,
    bool  is_sync_pressed
  ) {
    if(warning_seconds_ > 0) {
      warning_seconds_ -= delta_seconds;
      return std::nullopt;
    }

    needle_angle_ += delta_seconds * revolutions_per_second_ * ScalarMath::tau;
    if(is_sync_pressed) return judge();
    if(hasMissed()) return SkillCheckResultEnum::failed;

    return std::nullopt;
  }

  void SkillCheckMinigame::resolve(
    SkillCheckResultEnum result,
    RandomGenerator     &random
  ) {
    is_active_         = false;
    last_result_       = result;
    result_seconds_    = result_display_seconds;
    countdown_seconds_ = random.range(2.8F, 6.0F);
  }
} // namespace wheel_of_fear
