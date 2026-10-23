#pragma once

#include <optional>

#include "mods/mobs/Mob.hpp"
#include "mods/mobs/enums/MothStanceEnum.hpp"

namespace wheel_of_fear {
  class Moth final: public Mob {
    public:
      static constexpr MobTraitsProps species_traits {
        .display_name = "Lantern Moth",
        .introduction =
          "LANTERN MOTH  -  rare and friendly. Its glow holds the "
          "creatures still until a generator wakes.",
        .speed            = 0,
        .collision_radius = 5,
        .frozen_by        = std::nullopt,
        .sight_factor     = 0,
        .is_hostile       = false,
        .is_huggable      = false,
        .can_burn         = false,
        .first_phase      = 1,
        .spawn_weight     = 0,
      };

      explicit Moth(Vec2 position)
          : Mob(
              position,
              species_traits
            ) {
      }

      MobKindEnum kind() const override {
        return MobKindEnum::moth;
      }

      const MobTraitsProps &traits() const override {
        return species_traits;
      }

      MothStanceEnum stance() const {
        return stance_;
      }

      void emitLights(GameContextDto &context) override;
      void think(GameContextDto &context) override;
      void onGeneratorOnline(GameContextDto &context) override;
      void draw(GameContextDto &context) const override;

    private:
      static constexpr float befriend_distance      = 40;
      static constexpr float companion_light_radius = 125;
      static constexpr float departure_duration     = 2.5F;
      static constexpr Color light_color            = Color::hex(0xffe8a0);

      Vec2 flutter() const;
      void wait(GameContextDto &context);
      void follow(GameContextDto &context);
      void depart(GameContextDto &context);

      MothStanceEnum      stance_            = MothStanceEnum::waiting;
      float               departure_seconds_ = 0;
      std::optional<Vec2> home_;
  };
} // namespace wheel_of_fear
