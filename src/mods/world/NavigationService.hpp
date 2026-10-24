#pragma once

#include "mods/world/DistanceField.hpp"
#include <array>

namespace wheel_of_fear {
  class NavigationService {
    public:
      void invalidate();

      void refreshTowardPlayer(
        const TileMapModel &map,
        Vec2Props           player_position
      );

      [[nodiscard]]
      Vec2Props directionToPlayer(
        const TileMapModel &map,
        Vec2Props           from
      ) const;

      Vec2Props directionToward(
        const TileMapModel &map,
        Vec2Props           from,
        Vec2Props           target,
        float               elapsed_seconds
      );

    private:
      static constexpr std::size_t cached_field_count   = 10;
      static constexpr float       direct_path_distance = 24;

      DistanceField* fieldFor(
        const TileMapModel &map,
        Vec2Props           target
      );

      DistanceField                                 toward_player_;
      std::array<DistanceField, cached_field_count> cached_fields_ {};
      float                                         clock_seconds_ = 0;
  };
} // namespace wheel_of_fear
