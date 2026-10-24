#pragma once

#include "core/math/Vec2Props.hpp"
#include "mods/world/RoomLedPlacementProps.hpp"
#include "mods/world/TileMapModel.hpp"
#include <cstddef>
#include <vector>

namespace wheel_of_fear {
  class RoomLedStrip {
    public:
      static constexpr float inset                = 1.5F;
      static constexpr float maximum_piece_length = 2;

      static RoomLedStrip trace(
        const TileMapModel &map,
        std::size_t         room_index
      );

      [[nodiscard]]
      float length() const {
        return length_;
      }

      [[nodiscard]]
      Vec2Props pointAt(float distance_along) const;

      [[nodiscard]]
      Vec2Props inwardAt(float distance_along) const;

      [[nodiscard]]
      float nearestDistanceAlong(Vec2Props point) const;

      [[nodiscard]]
      std::vector<RoomLedPlacementProps>
      placements(const TileMapModel &map) const;

    private:
      static constexpr float passage_probe_distance = inset + 2;

      struct SegmentLocationProps {
          std::size_t segment = 0;
          float       along   = 0;
      };

      explicit RoomLedStrip(std::vector<Vec2Props> corners);

      [[nodiscard]]
      SegmentLocationProps locate(float distance_along) const;

      [[nodiscard]]
      Vec2Props outwardAt(float distance_along) const;

      [[nodiscard]]
      Vec2Props segmentDirection(std::size_t segment) const;

      std::vector<Vec2Props> corners_;
      std::vector<float>     segment_lengths_;
      float                  length_ = 0;
  };
} // namespace wheel_of_fear
