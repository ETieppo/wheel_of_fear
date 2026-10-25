#pragma once

#include "mods/mobs/MobEntity.hpp"

namespace wheel_of_fear {
  class ShepherdMob final: public MobEntity {
    public:
      static constexpr MobTraitsProps species_traits {
        .display_name            = {"Shepherd", "Pastor"},
        .frozen_by               = LightFilterEnum::any_light,
        .movement_sound          = SoundEnum::shepherd_step,
        .first_phase             = 3,
        .speed                   = 38,
        .collision_radius        = 8,
        .sight_factor            = 1.25F,
        .field_of_view_cosine    = 0.35F,
        .movement_sound_volume   = 0.34F,
        .movement_sound_interval = 0.7F,
        .spawn_weight            = 0.3F,
      };

      explicit ShepherdMob(Vec2Props position)
          : MobEntity(
              position,
              species_traits
            ) {
      }

      [[nodiscard]]
      MobKindEnum kind() const override {
        return MobKindEnum::shepherd;
      }

      [[nodiscard]]
      const MobTraitsProps &traits() const override {
        return species_traits;
      }

      void think(GameContextDto &context) override;

      void draw(GameContextDto &context) const override;

    private:
      static constexpr float preferred_distance    = 135;
      static constexpr float call_interval_seconds = 8;
      static constexpr float call_seconds          = 1.6F;
      static constexpr float call_radius           = 460;
      static constexpr float call_frenzy_seconds   = 5;
      static constexpr float call_ripple_seconds   = 1.4F;

      void call(GameContextDto &context);

      float seconds_since_call_ = call_interval_seconds;
  };
} // namespace wheel_of_fear
