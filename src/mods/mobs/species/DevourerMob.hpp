#pragma once

#include "mods/mobs/MobEntity.hpp"
#include <optional>

namespace wheel_of_fear {
  class DevourerMob final: public MobEntity {
    public:
      static constexpr MobTraitsProps species_traits {
        .display_name        = {"Devourer", "Devorador"},
        .frozen_by           = LightFilterEnum::any_light,
        .vocal_sound         = SoundEnum::jaws,
        .first_phase         = 12,
        .speed               = 46,
        .collision_radius    = 9,
        .vocal_volume        = 0.8F,
        .can_burn            = false,
        .avoids_hazards      = false,
        .spawn_weight        = 0.25F,
        .is_lured_by_beacons = false,
      };

      explicit DevourerMob(Vec2Props position)
          : MobEntity(
              position,
              species_traits
            ) {
      }

      [[nodiscard]]
      MobKindEnum kind() const override {
        return MobKindEnum::devourer;
      }

      [[nodiscard]]
      const MobTraitsProps &traits() const override {
        return species_traits;
      }

      void think(GameContextDto &context) override;

      void onContact(GameContextDto &context) override;

      void draw(GameContextDto &context) const override;

    private:
      static constexpr float tool_scent_range       = 520;
      static constexpr float bite_reach             = 16;
      static constexpr float strike_retreat_seconds = 1.4F;

      void huntTool(
        GameContextDto &context,
        Vec2Props       tool
      );
  };
} // namespace wheel_of_fear
