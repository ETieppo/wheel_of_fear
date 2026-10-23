#pragma once

#include <cstdint>
#include <vector>

#include "core/math/Vec2.hpp"
#include "mods/world/TileCoordinateProps.hpp"
#include "mods/world/TileMap.hpp"

namespace wheel_of_fear {
  class DistanceField {
    public:
      static constexpr std::uint16_t unreachable_distance = 0xFFFF;

      bool isFreshFor(
        const TileMap      &map,
        TileCoordinateProps target_tile
      ) const;
      void compute(
        const TileMap &map,
        Vec2           target
      );
      std::uint16_t distanceAt(
        const TileMap &map,
        Vec2           point
      ) const;
      Vec2 directionAt(
        const TileMap &map,
        Vec2           point
      ) const;

      void invalidate() {
        map_version_ = 0;
      }

      float lastUsedSeconds() const {
        return last_used_seconds_;
      }

      void markUsed(float seconds) {
        last_used_seconds_ = seconds;
      }

    private:
      std::vector<std::uint16_t> distances_;
      std::vector<std::uint32_t> queue_;
      TileCoordinateProps        target_tile_ {-1, -1};
      std::uint64_t              map_version_       = 0;
      float                      last_used_seconds_ = 0;
  };
} // namespace wheel_of_fear
