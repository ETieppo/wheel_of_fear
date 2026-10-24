#pragma once

#include "core/math/Vec2Props.hpp"
#include "mods/world/TileCoordinateProps.hpp"
#include "mods/world/TileMapModel.hpp"
#include <cstdint>
#include <vector>

namespace wheel_of_fear {
  class DistanceField {
    public:
      static constexpr std::uint16_t unreachable_distance = 0xFFFF;

      [[nodiscard]]
      bool isFreshFor(
        const TileMapModel &map,
        TileCoordinateProps target_tile
      ) const;

      void compute(
        const TileMapModel &map,
        Vec2Props           target
      );

      [[nodiscard]]
      std::uint16_t distanceAt(
        const TileMapModel &map,
        Vec2Props           point
      ) const;

      [[nodiscard]]
      Vec2Props directionAt(
        const TileMapModel &map,
        Vec2Props           point
      ) const;

      void invalidate() {
        map_version_ = 0;
      }

      [[nodiscard]]
      float lastUsedSeconds() const {
        return last_used_seconds_;
      }

      void markUsed(float seconds) {
        last_used_seconds_ = seconds;
      }

    private:
      std::vector<std::uint16_t> distances_;
      std::vector<std::uint32_t> queue_;
      TileCoordinateProps        target_tile_ {.x = -1, .y = -1};
      std::uint64_t              map_version_       = 0;
      float                      last_used_seconds_ = 0;
  };
} // namespace wheel_of_fear
