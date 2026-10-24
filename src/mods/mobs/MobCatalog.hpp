#pragma once

#include <memory>

#include "core/math/Vec2Props.hpp"
#include "mods/mobs/MobEntity.hpp"
#include "mods/mobs/MobTraitsProps.hpp"
#include "mods/mobs/enums/MobKindEnum.hpp"

namespace wheel_of_fear {
  class MobCatalog final {
    public:
      MobCatalog() = delete;

      static const MobTraitsProps &traitsOf(
        MobKindEnum kind
      );
      static std::unique_ptr<MobEntity> create(
        MobKindEnum kind,
        Vec2Props   position
      );
  };
} // namespace wheel_of_fear
