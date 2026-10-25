#pragma once

#include "mods/mobs/MobEntity.hpp"
#include "mods/mobs/enums/MothStanceEnum.hpp"
#include "mods/world/TileMapModel.hpp"
#include <optional>

namespace wheel_of_fear {
  class MothMob final: public MobEntity {
    public:
      static constexpr MobTraitsProps species_traits {
        .display_name         = {"Lantern Moth", "Mariposa-Lanterna"},
        .frozen_by            = std::nullopt,
        .first_phase          = 1,
        .speed                = 0,
        .collision_radius     = 5,
        .sight_factor         = 0,
        .is_hostile           = false,
        .is_huggable          = false,
        .can_burn             = false,
        .passes_through_walls = true,
        .spawn_weight         = 0,
      };

      explicit MothMob(Vec2Props position)
          : MobEntity(
              position,
              species_traits
            ) {
      }

      [[nodiscard]]
      MobKindEnum kind() const override {
        return MobKindEnum::moth;
      }

      [[nodiscard]]
      const MobTraitsProps &traits() const override {
        return species_traits;
      }

      [[nodiscard]]
      MothStanceEnum stance() const {
        return stance_;
      }

      void emitLights(GameContextDto &context) override;

      void think(GameContextDto &context) override;

      void onGeneratorOnline(GameContextDto &context) override;

      void draw(GameContextDto &context) const override;

    private:
      static constexpr float      befriend_distance      = 40;
      static constexpr float      companion_light_radius = 125;
      static constexpr float      departure_duration     = 2.5F;
      static constexpr float      light_probe_step       = 2;
      static constexpr ColorProps light_color = ColorProps::hex(0xffe8a0);

      [[nodiscard]]
      Vec2Props flutter() const;

      [[nodiscard]]
      Vec2Props lightSource(
        const TileMapModel &map,
        Vec2Props           toward
      ) const;

      void wait(GameContextDto &context);

      void follow(GameContextDto &context);

      void depart(GameContextDto &context);

      MothStanceEnum           stance_            = MothStanceEnum::waiting;
      float                    departure_seconds_ = 0;
      std::optional<Vec2Props> home_;
  };
} // namespace wheel_of_fear
