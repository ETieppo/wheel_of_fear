#pragma once

#include "mods/mobs/MobEntity.hpp"

namespace wheel_of_fear {
  class WatcherMob final: public MobEntity {
    public:
      static constexpr MobTraitsProps species_traits {
        .display_name         = "Watcher",
        .frozen_by            = std::nullopt,
        .vocal_sound          = SoundEnum::distant_scream,
        .first_phase          = 11,
        .speed                = 26,
        .collision_radius     = 9,
        .sight_factor         = 1.6F,
        .field_of_view_cosine = -0.2F,
        .vocal_volume         = 0.9F,
        .spawn_weight         = 0.2F,
      };

      explicit WatcherMob(Vec2Props position)
          : MobEntity(
              position,
              species_traits
            ) {
      }

      [[nodiscard]]
      MobKindEnum kind() const override {
        return MobKindEnum::watcher;
      }

      [[nodiscard]]
      const MobTraitsProps &traits() const override {
        return species_traits;
      }

      void think(GameContextDto &context) override;

      void onContact([[maybe_unused]] GameContextDto &context) override {
      }

      void draw(GameContextDto &context) const override;

    private:
      static constexpr float scream_cooldown_seconds = 8;
      static constexpr float scream_flash_seconds    = 0.8F;
      static constexpr float scream_frenzy_seconds   = 6;
      static constexpr float sector_wide_radius      = 100000;
      static constexpr float keep_away_distance      = 110;

      void scream(GameContextDto &context);
      void keepWatching(GameContextDto &context);

      float seconds_since_scream_ = scream_cooldown_seconds;
  };
} // namespace wheel_of_fear
