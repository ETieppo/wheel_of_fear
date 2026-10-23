#include "mods/generators/SkillCheck.hpp"

#include <algorithm>

#include "core/math/Scalar.hpp"

namespace wheel_of_fear {

SkillCheck::SkillCheck(float zone_start, float zone_width, float perfect_width) : zone_start_(zone_start), zone_width_(zone_width), perfect_width_(perfect_width) {}

SkillCheckResultEnum SkillCheck::judge() const {
    if (needle_angle_ < zone_start_ || needle_angle_ > zoneEnd()) return SkillCheckResultEnum::failed;
    const float perfect_start = perfectStart();
    if (needle_angle_ >= perfect_start && needle_angle_ <= perfect_start + perfect_width_) return SkillCheckResultEnum::perfect;
    return SkillCheckResultEnum::good;
}

bool SkillCheck::hasMissed() const {
    return needle_angle_ > zoneEnd() + 0.05F || needle_angle_ >= Scalar::tau;
}

void SkillCheck::tickResult(float delta_seconds) {
    result_seconds_ = std::max(0.0F, result_seconds_ - delta_seconds);
}

bool SkillCheck::tickCountdown(float delta_seconds) {
    countdown_seconds_ -= delta_seconds;
    return countdown_seconds_ <= 0;
}

void SkillCheck::begin(std::uint32_t phase, RandomGenerator& random) {
    const auto difficulty = static_cast<float>(std::min(phase, 6U));
    is_active_ = true;
    warning_seconds_ = warning_duration_seconds;
    needle_angle_ = 0;
    zone_start_ = random.range(1.7F, 5.0F);
    zone_width_ = std::max(0.5F, 1.0F - difficulty * 0.07F);
    perfect_width_ = 0.22F;
    revolutions_per_second_ = 0.75F + difficulty * 0.06F;
    countdown_seconds_ = 2.5F;
}

std::optional<SkillCheckResultEnum> SkillCheck::advanceNeedle(float delta_seconds, bool is_sync_pressed) {
    if (warning_seconds_ > 0) {
        warning_seconds_ -= delta_seconds;
        return std::nullopt;
    }
    needle_angle_ += delta_seconds * revolutions_per_second_ * Scalar::tau;
    if (is_sync_pressed) return judge();
    if (hasMissed()) return SkillCheckResultEnum::failed;
    return std::nullopt;
}

void SkillCheck::resolve(SkillCheckResultEnum result, RandomGenerator& random) {
    is_active_ = false;
    last_result_ = result;
    result_seconds_ = result_display_seconds;
    countdown_seconds_ = random.range(2.8F, 6.0F);
}

}
