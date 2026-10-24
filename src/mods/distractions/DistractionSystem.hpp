#pragma once

#include <vector>

#include "core/math/Vec2Props.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/distractions/DistractionEvent.hpp"
#include "mods/distractions/enums/DistractionKindEnum.hpp"

namespace wheel_of_fear {
  class MobEntity;

  class DistractionSystem {
    public:
      void clear() {
        active_.clear();
      }

      void update(
        GameContextDto &context
      );
      void provoke(
        GameContextDto     &context,
        DistractionKindEnum kind,
        Vec2Props           position
      );

    private:
      static constexpr float scenery_contact_distance      = 15;
      static constexpr float fight_contact_distance        = 9;
      static constexpr float fight_push_speed              = 160;
      static constexpr float mob_incident_cooldown_seconds = 4;

      bool checkSceneryContact(
        GameContextDto &context,
        MobEntity      &mob
      );
      void checkMobFight(
        GameContextDto &context,
        MobEntity      &mob,
        std::size_t     first_other_index
      );
      void advanceFlashes(
        GameContextDto &context
      );

      std::vector<DistractionEvent> active_;
  };
} // namespace wheel_of_fear
