#pragma once

#include "mods/mobs/MobEntity.hpp"
#include "mods/mobs/enums/MimicStanceEnum.hpp"

namespace wheel_of_fear {
  class MimicMob final: public MobEntity {
    public:
      static constexpr float lunge_speed = 90;

      static constexpr MobTraitsProps species_traits {
        .display_name            = "Mimic",
        .frozen_by               = LightFilterEnum::any_light,
        .movement_sound          = SoundEnum::mimic_clank,
        .vocal_sound             = SoundEnum::jaws,
        .first_phase             = 5,
        .speed                   = 46,
        .collision_radius        = 9,
        .strike_reach            = 13,
        .movement_sound_volume   = 0.36F,
        .movement_sound_interval = 0.5F,
        .vocal_volume            = 0.7F,
        .spawn_weight            = 0,
      };

      explicit MimicMob(Vec2Props position)
          : MobEntity(
              position,
              species_traits
            ) {
      }

      [[nodiscard]]
      MobKindEnum kind() const override {
        return MobKindEnum::mimic;
      }

      [[nodiscard]]
      const MobTraitsProps &traits() const override {
        return species_traits;
      }

      [[nodiscard]]
      bool isDisguised() const override {
        return stance_ == MimicStanceEnum::disguised;
      }

      void think(GameContextDto &context) override;

      void onContact(GameContextDto &context) override;

      void draw(GameContextDto &context) const override;

    private:
      static constexpr float wake_distance          = 46;
      static constexpr float lunge_duration         = 0.7F;
      static constexpr float strike_retreat_seconds = 0.6F;

      void waitInDisguise(GameContextDto &context);

      void lunge(GameContextDto &context);

      MimicStanceEnum stance_        = MimicStanceEnum::disguised;
      float           lunge_seconds_ = 0;
  };
} // namespace wheel_of_fear
