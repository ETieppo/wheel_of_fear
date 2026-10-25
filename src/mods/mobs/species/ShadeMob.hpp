#pragma once

#include "core/math/ScalarMath.hpp"
#include "mods/mobs/MobEntity.hpp"

namespace wheel_of_fear {
  class ShadeMob final: public MobEntity {
    public:
      static constexpr MobTraitsProps species_traits {
        .display_name         = {"Shade", "Sombra"},
        .frozen_by            = LightFilterEnum::ultraviolet_only,
        .vocal_sound          = SoundEnum::whisper,
        .first_phase          = 4,
        .speed                = 44,
        .collision_radius     = 7,
        .visible_under        = LightFilterEnum::ultraviolet_only,
        .field_of_view_cosine = 0,
        .vocal_volume         = 0.55F,
        .spawn_weight         = 0.35F,
      };

      explicit ShadeMob(Vec2Props position)
          : MobEntity(
              position,
              species_traits
            ) {
      }

      [[nodiscard]]
      MobKindEnum kind() const override {
        return MobKindEnum::shade;
      }

      [[nodiscard]]
      const MobTraitsProps &traits() const override {
        return species_traits;
      }

      void think(GameContextDto &context) override;

      void onContact(GameContextDto &context) override;

      void draw(GameContextDto &context) const override;

    private:
      static constexpr float sprite_facing_offset   = ScalarMath::tau / 2;
      static constexpr float strike_retreat_seconds = 1.6F;
      static constexpr float whisper_distance       = 70;
  };
} // namespace wheel_of_fear
