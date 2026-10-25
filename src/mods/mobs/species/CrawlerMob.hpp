#pragma once

#include "mods/mobs/MobEntity.hpp"

namespace wheel_of_fear {
  class CrawlerMob final: public MobEntity {
    public:
      static constexpr MobTraitsProps species_traits {
        .display_name            = "Crawler",
        .frozen_by               = LightFilterEnum::any_light,
        .movement_sound          = SoundEnum::crawler_skitter,
        .vocal_sound             = SoundEnum::crawler_chitter,
        .first_phase             = 1,
        .speed                   = 62,
        .collision_radius        = 7,
        .field_of_view_cosine    = 0.1F,
        .hearing_factor          = 1.15F,
        .strike_reach            = 11,
        .movement_sound_volume   = 0.26F,
        .movement_sound_interval = 0.34F,
        .vocal_volume            = 0.55F,
        .spawn_weight            = 1.0F,
        .has_body_collision      = true,
      };

      explicit CrawlerMob(Vec2Props position)
          : MobEntity(
              position,
              species_traits
            ) {
      }

      [[nodiscard]]
      MobKindEnum kind() const override {
        return MobKindEnum::crawler;
      }

      [[nodiscard]]
      const MobTraitsProps &traits() const override {
        return species_traits;
      }

      void onContact(GameContextDto &context) override;

      void onLightReleased(GameContextDto &context) override;

      void draw(GameContextDto &context) const override;

    private:
      static constexpr float strike_retreat_seconds = 1.4F;
      static constexpr float flinch_seconds         = 0.7F;
  };
} // namespace wheel_of_fear
