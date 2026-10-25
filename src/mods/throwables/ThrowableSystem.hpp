#pragma once

#include "core/items/enums/ItemEnum.hpp"
#include "core/math/ColorProps.hpp"
#include "core/math/Vec2Props.hpp"
#include "core/theme/Palette.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/throwables/LureBeaconEntity.hpp"
#include "mods/throwables/NeonPuddleHazard.hpp"
#include "mods/throwables/PickupEntity.hpp"
#include "mods/throwables/projectiles/Projectile.hpp"
#include <memory>
#include <vector>

namespace wheel_of_fear {
  class ThrowableSystem {
    public:
      ThrowableSystem();
      ~ThrowableSystem();

      void generatePhase(WorldSystem &world);

      void launch(
        ItemEnum  item,
        Vec2Props from,
        Vec2Props toward
      );

      void deployBeacon(Vec2Props landing) {
        beacons_.emplace_back(landing);
      }

      void spillPuddle(Vec2Props landing) {
        puddles_.emplace_back(landing, grenade_color_);
      }

      [[nodiscard]]
      ColorProps grenadeColor() const {
        return grenade_color_;
      }

      void update(GameContextDto &context);

      void draw(GameContextDto &context) const;

      [[nodiscard]]
      const std::vector<LureBeaconEntity> &beacons() const {
        return beacons_;
      }

      [[nodiscard]]
      bool isBeaconActiveAt(Vec2Props position) const;

      [[nodiscard]]
      std::size_t puddleCount() const {
        return puddles_.size();
      }

    private:
      static constexpr float maximum_throw_distance = 190;
      static constexpr float beacon_match_distance  = 1;

      void emitLights(GameContextDto &context) const;

      void advanceProjectiles(GameContextDto &context);

      void advanceBeacons(GameContextDto &context);

      void advancePuddles(float delta_seconds);

      void collectPickups(GameContextDto &context);

      std::vector<std::unique_ptr<Projectile>> projectiles_;
      std::vector<LureBeaconEntity>            beacons_;
      std::vector<NeonPuddleHazard>            puddles_;
      std::vector<PickupEntity>                pickups_;
      ColorProps grenade_color_ = Palette::grenade_neon_colors.front();
  };
} // namespace wheel_of_fear
