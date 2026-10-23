#pragma once

#include <array>

#include "mods/world/DistanceField.hpp"

namespace wheel_of_fear {
  class Navigation {
    public:
      void invalidate();
      void refreshTowardPlayer(
        const TileMap &map,
        Vec2           player_position
      );
      Vec2 directionToPlayer(
        const TileMap &map,
        Vec2           from
      ) const;
      Vec2 directionToward(
        const TileMap &map,
        Vec2           from,
        Vec2           target,
        float          elapsed_seconds
      );

    private:
      static constexpr std::size_t cached_field_count   = 10;
      static constexpr float       direct_path_distance = 24;

      DistanceField* fieldFor(
        const TileMap &map,
        Vec2           target
      );

      DistanceField                                 toward_player_;
      std::array<DistanceField, cached_field_count> cached_fields_ {};
      float                                         clock_seconds_ = 0;
  };
} // namespace wheel_of_fear
