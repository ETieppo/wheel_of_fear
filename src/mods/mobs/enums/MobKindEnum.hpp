#pragma once

#include <array>

namespace wheel_of_fear {

enum class MobKindEnum { crawler, bloater, melter, shepherd, shade, mimic, moth };

inline constexpr std::array<MobKindEnum, 7> all_mob_kinds{
    MobKindEnum::crawler,
    MobKindEnum::bloater,
    MobKindEnum::melter,
    MobKindEnum::shepherd,
    MobKindEnum::shade,
    MobKindEnum::mimic,
    MobKindEnum::moth,
};

}
