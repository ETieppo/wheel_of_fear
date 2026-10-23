#pragma once

#include <vector>

#include "core/math/Vec2.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/distractions/Distraction.hpp"
#include "mods/distractions/enums/DistractionKindEnum.hpp"

namespace wheel_of_fear {
  class Mob;

  class Distractions {
    public:
      void clear() {
        active_.clear();
      }

      void update(GameContextDto &context);
      void provoke(
        GameContextDto     &context,
        DistractionKindEnum kind,
        Vec2                position
      );

    private:
      static constexpr float scenery_contact_distance      = 15;
      static constexpr float fight_contact_distance        = 9;
      static constexpr float fight_push_speed              = 160;
      static constexpr float mob_incident_cooldown_seconds = 4;

      bool checkSceneryContact(
        GameContextDto &context,
        Mob            &mob
      );
      void checkMobFight(
        GameContextDto &context,
        Mob            &mob,
        std::size_t     first_other_index
      );
      void advanceFlashes(GameContextDto &context);

      std::vector<Distraction> active_;
  };
} // namespace wheel_of_fear
