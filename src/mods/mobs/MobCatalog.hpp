#pragma once

#include <memory>

#include "core/math/Vec2.hpp"
#include "mods/mobs/Mob.hpp"
#include "mods/mobs/MobTraitsProps.hpp"
#include "mods/mobs/enums/MobKindEnum.hpp"

namespace wheel_of_fear {
  class MobCatalog final {
    public:
      MobCatalog() = delete;

      static const MobTraitsProps &traitsOf(
        MobKindEnum kind
      );
      static std::unique_ptr<Mob> create(
        MobKindEnum kind,
        Vec2        position
      );
  };
} // namespace wheel_of_fear
