#pragma once

#include "mods/mobs/Mob.hpp"
#include "mods/mobs/enums/MimicStanceEnum.hpp"

namespace wheel_of_fear {
  class Mimic final: public Mob {
    public:
      static constexpr MobTraitsProps species_traits {
        .display_name = "Mimic",
        .introduction =
          "MIMIC  -  it wears the shell of a fabricator. The tracker "
          "can't tell the difference. Neither can you.",
        .speed                   = 58,
        .collision_radius        = 9,
        .frozen_by               = LightFilterEnum::any_light,
        .strike_reach            = 13,
        .movement_sound          = SoundEnum::mimic_clank,
        .movement_sound_volume   = 0.36F,
        .movement_sound_interval = 0.5F,
        .vocal_sound             = SoundEnum::jaws,
        .vocal_volume            = 0.7F,
        .first_phase             = 5,
        .spawn_weight            = 0,
      };

      explicit Mimic(Vec2 position)
          : Mob(
              position,
              species_traits
            ) {
      }

      MobKindEnum kind() const override {
        return MobKindEnum::mimic;
      }

      const MobTraitsProps &traits() const override {
        return species_traits;
      }

      bool isDisguised() const override {
        return stance_ == MimicStanceEnum::disguised;
      }

      void think(GameContextDto &context) override;
      void onContact(GameContextDto &context) override;
      void draw(GameContextDto &context) const override;

    private:
      static constexpr float wake_distance          = 46;
      static constexpr float lunge_speed            = 150;
      static constexpr float lunge_duration         = 0.7F;
      static constexpr float strike_retreat_seconds = 0.6F;

      void waitInDisguise(GameContextDto &context);
      void lunge(GameContextDto &context);

      MimicStanceEnum stance_        = MimicStanceEnum::disguised;
      float           lunge_seconds_ = 0;
  };
} // namespace wheel_of_fear
