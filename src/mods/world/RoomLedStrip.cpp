#include "mods/world/RoomLedStrip.hpp"

#include <cstdint>
#include <optional>
#include <unordered_map>

#include "core/math/ScalarMath.hpp"
#include "core/render/RenderConfig.hpp"

namespace wheel_of_fear {
  namespace {
    std::uint64_t latticeKey(
      TileCoordinateProps point
    ) {
      const auto row =
        static_cast<std::uint64_t>(static_cast<std::uint32_t>(point.y));

      const auto column =
        static_cast<std::uint64_t>(static_cast<std::uint32_t>(point.x));

      return (row << 32U) | column;
    }

    Vec2Props inwardNormal(
      Vec2Props direction
    ) {
      return Vec2Props::of(-direction.y, direction.x);
    }

    Vec2Props latticeToWorld(
      TileCoordinateProps point
    ) {
      return Vec2Props::of(
        static_cast<float>(point.x) * RenderConfig::tile_size,
        static_cast<float>(point.y) * RenderConfig::tile_size
      );
    }

    std::
      unordered_map<
        std::uint64_t,
        TileCoordinateProps
      >
      boundaryEdges(
        const TileMapModel &map,
        std::size_t         room_index
      ) {
      const RoomProps &room = map.rooms()[room_index];

      const auto isRoomTile = [&map, room_index](int x, int y) {
        return map.roomIndexAtTile(x, y) == room_index;
      };

      std::unordered_map<std::uint64_t, TileCoordinateProps> next_corner;

      const auto addEdge =
        [&next_corner](TileCoordinateProps from, TileCoordinateProps to) {
          next_corner[latticeKey(from)] = to;
        };

      for(int y = room.top; y < room.top + room.height; ++y) {
        for(int x = room.left; x < room.left + room.width; ++x) {
          if(!isRoomTile(x, y)) continue;

          if(!isRoomTile(x, y - 1))
            addEdge({.x = x, .y = y}, {.x = x + 1, .y = y});

          if(!isRoomTile(x + 1, y))
            addEdge({.x = x + 1, .y = y}, {.x = x + 1, .y = y + 1});

          if(!isRoomTile(x, y + 1))
            addEdge({.x = x + 1, .y = y + 1}, {.x = x, .y = y + 1});

          if(!isRoomTile(x - 1, y))
            addEdge({.x = x, .y = y + 1}, {.x = x, .y = y});
        }
      }

      return next_corner;
    }

    TileCoordinateProps firstRoomCorner(
      const TileMapModel &map,
      std::size_t         room_index
    ) {
      const RoomProps &room = map.rooms()[room_index];

      for(int y = room.top; y < room.top + room.height; ++y)
        for(int x = room.left; x < room.left + room.width; ++x)
          if(map.roomIndexAtTile(x, y) == room_index) return {.x = x, .y = y};

      return {.x = room.left, .y = room.top};
    }
  } // namespace

  RoomLedStrip RoomLedStrip::trace(
    const TileMapModel &map,
    std::size_t         room_index
  ) {
    const auto                next_corner = boundaryEdges(map, room_index);
    const TileCoordinateProps start       = firstRoomCorner(map, room_index);
    std::vector<TileCoordinateProps> lattice_path {start};
    TileCoordinateProps              current = start;

    for(std::size_t step = 0; step < next_corner.size(); ++step) {
      const auto found = next_corner.find(latticeKey(current));
      if(found == next_corner.end()) break;

      current = found->second;
      if(current == start) break;

      lattice_path.push_back(current);
    }

    std::vector<Vec2Props> corners;
    const std::size_t      count = lattice_path.size();

    for(std::size_t index = 0; index < count; ++index) {
      const Vec2Props previous =
        latticeToWorld(lattice_path[(index + count - 1) % count]);

      const Vec2Props here = latticeToWorld(lattice_path[index]);

      const Vec2Props following =
        latticeToWorld(lattice_path[(index + 1) % count]);

      const Vec2Props incoming = here.sub(previous).normalized();
      const Vec2Props outgoing = following.sub(here).normalized();
      if(incoming.dot(outgoing) > 0.5F) continue;

      corners.push_back(here.add(
        inwardNormal(incoming).add(inwardNormal(outgoing)).scale(inset)
      ));
    }

    return RoomLedStrip(std::move(corners));
  }

  RoomLedStrip::RoomLedStrip(
    std::vector<Vec2Props> corners
  )
      : corners_(std::move(corners)) {
    for(std::size_t index = 0; index < corners_.size(); ++index) {
      const float segment_length =
        corners_[index].distance(corners_[(index + 1) % corners_.size()]);

      segment_lengths_.push_back(segment_length);
      length_ += segment_length;
    }
  }

  RoomLedStrip::SegmentLocationProps RoomLedStrip::locate(
    float distance_along
  ) const {
    float remaining = ScalarMath::floorModulo(distance_along, length_);

    for(std::size_t segment = 0; segment < segment_lengths_.size(); ++segment) {
      if(remaining < segment_lengths_[segment])
        return {.segment = segment, .along = remaining};

      remaining -= segment_lengths_[segment];
    }

    return {.segment = 0, .along = 0};
  }

  Vec2Props RoomLedStrip::segmentDirection(
    std::size_t segment
  ) const {
    return corners_[(segment + 1) % corners_.size()]
      .sub(corners_[segment])
      .normalized();
  }

  Vec2Props RoomLedStrip::pointAt(
    float distance_along
  ) const {
    const SegmentLocationProps location = locate(distance_along);

    return corners_[location.segment].add(
      segmentDirection(location.segment).scale(location.along)
    );
  }

  Vec2Props RoomLedStrip::inwardAt(
    float distance_along
  ) const {
    return inwardNormal(segmentDirection(locate(distance_along).segment));
  }

  Vec2Props RoomLedStrip::outwardAt(
    float distance_along
  ) const {
    const SegmentLocationProps location = locate(distance_along);
    const std::size_t          count    = corners_.size();
    const Vec2Props inward = inwardNormal(segmentDirection(location.segment));
    std::optional<std::size_t> neighbor;

    if(location.along <= inset)
      neighbor = (location.segment + count - 1) % count;
    else if(segment_lengths_[location.segment] - location.along <= inset)
      neighbor = (location.segment + 1) % count;
    if(!neighbor) return inward.scale(-1);

    return inward.add(inwardNormal(segmentDirection(*neighbor)))
      .normalized()
      .scale(-1);
  }

  std::vector<RoomLedPlacementProps> RoomLedStrip::placements(
    const TileMapModel &map
  ) const {
    std::vector<RoomLedPlacementProps> leds;
    const auto led_count = static_cast<int>(length_ / led_spacing);

    for(int led = 0; led < led_count; ++led) {
      const float     distance = static_cast<float>(led) * led_spacing;
      const Vec2Props position = pointAt(distance);
      const Vec2Props outward  = outwardAt(distance);

      leds.push_back(
        RoomLedPlacementProps {
          .distance_along = distance,
          .position       = position,
          .outward        = outward,
          .faces_open_passage =
            !map.isWallAt(position.add(outward.scale(passage_probe_distance))),
          .is_covered_by_object = map.isWallAt(position),
        }
      );
    }

    return leds;
  }
} // namespace wheel_of_fear
