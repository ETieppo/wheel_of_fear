#pragma once

#include "mods/mobs/Mob.hpp"

namespace wheel_of_fear {
  class Shade final: public Mob {
    public:
      static constexpr MobTraitsProps species_traits {
        .display_name = "Shade",
        .introduction = "SHADE  -  a silent smear of dark. Only ultraviolet "
                        "reveals it, and only ultraviolet holds it.",
        .speed        = 56,
        .collision_radius     = 7,
        .frozen_by            = LightFilterEnum::ultraviolet_only,
        .visible_under        = LightFilterEnum::ultraviolet_only,
        .field_of_view_cosine = 0,
        .vocal_sound          = SoundEnum::whisper,
        .vocal_volume         = 0.55F,
        .first_phase          = 4,
        .spawn_weight         = 0.35F,
      };

      explicit Shade(
        Vec2 position
      )
          : Mob(
              position,
              species_traits
            ) {
      }

      MobKindEnum kind() const override {
        return MobKindEnum::shade;
      }

      const MobTraitsProps &traits() const override {
        return species_traits;
      }

      void think(
        GameContextDto &context
      ) override;
      void onContact(
        GameContextDto &context
      ) override;
      void draw(
        GameContextDto &context
      ) const override;

    private:
      static constexpr float strike_retreat_seconds = 1.6F;
      static constexpr float whisper_distance       = 70;
  };
} // namespace wheel_of_fear
