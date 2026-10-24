#pragma once

#include "mods/mobs/MobEntity.hpp"

namespace wheel_of_fear {
  class BloaterMob final: public MobEntity {
    public:
      static constexpr MobTraitsProps species_traits {
        .display_name = "Bloater",
        .introduction
        = "BLOATER  -  a blind gas sac that drifts toward noise. Ultraviolet "
          "lights its fuse; it takes everything nearby with it.",
        .speed                   = 20,
        .collision_radius        = 9,
        .frozen_by               = LightFilterEnum::any_light,
        .sight_factor            = 0,
        .hearing_factor          = 1.6F,
        .strike_reach            = 13,
        .movement_sound          = SoundEnum::bloater_gurgle,
        .movement_sound_volume   = 0.32F,
        .movement_sound_interval = 1.9F,
        .first_phase             = 2,
        .spawn_weight            = 0.55F,
      };

      explicit BloaterMob(Vec2Props position)
          : MobEntity(
              position,
              species_traits
            ) {
      }

      [[nodiscard]]
      MobKindEnum kind() const override {
        return MobKindEnum::bloater;
      }

      [[nodiscard]]
      const MobTraitsProps &traits() const override {
        return species_traits;
      }

      void think(GameContextDto &context) override;

      void onContact(GameContextDto &context) override;

      void ignite(GameContextDto &context) override;

      void catchBlast(GameContextDto &context) override;

      void draw(GameContextDto &context) const override;

    private:
      static constexpr float fuse_limit_seconds             = 1.3F;
      static constexpr float ultraviolet_ignition_threshold = 0.18F;
      static constexpr float chained_fuse_seconds           = 0.9F;

      [[nodiscard]]
      bool isLit() const {
        return fuse_seconds_ > 0;
      }

      void lightFuse(float minimum_fuse_seconds);

      float fuse_seconds_ = 0;
  };
} // namespace wheel_of_fear
