#include "mods/world/DistanceField.hpp"

#include <algorithm>
#include <array>

namespace wheel_of_fear {
  namespace {
    constexpr std::array<TileCoordinateProps, 4> four_neighbors {
      {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}
    };
    constexpr std::array<TileCoordinateProps, 8> eight_neighbors {
      {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1}}
    };
  } // namespace

  bool DistanceField::isFreshFor(
    const TileMap      &map,
    TileCoordinateProps target_tile
  ) const {
    return map_version_ == map.version() && target_tile_ == target_tile;
  }

  void DistanceField::compute(
    const TileMap &map,
    Vec2           target
  ) {
    const auto cell_count =
      static_cast<std::size_t>(map.width() * map.height());
    distances_.assign(cell_count, unreachable_distance);
    const TileCoordinateProps target_tile = TileMap::tileAt(target);
    target_tile_                          = target_tile;
    map_version_                          = map.version();
    if(!map.contains(target_tile.x, target_tile.y)) return;
    queue_.clear();
    const std::size_t start = map.cellIndex(target_tile.x, target_tile.y);
    queue_.push_back(static_cast<std::uint32_t>(start));
    distances_[start] = 0;
    const auto width  = static_cast<std::uint32_t>(map.width());
    for(std::size_t head = 0; head < queue_.size(); ++head) {
      const std::uint32_t cell     = queue_[head];
      const auto          x        = static_cast<int>(cell % width);
      const auto          y        = static_cast<int>(cell / width);
      const std::uint16_t distance = distances_[cell];
      for(const TileCoordinateProps &offset : four_neighbors) {
        const int neighbor_x = x + offset.x;
        const int neighbor_y = y + offset.y;
        if(map.isBlocked(neighbor_x, neighbor_y)) continue;
        const std::size_t neighbor = map.cellIndex(neighbor_x, neighbor_y);
        if(distances_[neighbor] != unreachable_distance) continue;
        distances_[neighbor] = static_cast<std::uint16_t>(distance + 1);
        queue_.push_back(static_cast<std::uint32_t>(neighbor));
      }
    }
  }

  std::uint16_t DistanceField::distanceAt(
    const TileMap &map,
    Vec2           point
  ) const {
    const TileCoordinateProps tile = TileMap::tileAt(point);
    if(!map.contains(tile.x, tile.y) || distances_.empty())
      return unreachable_distance;
    return distances_[map.cellIndex(tile.x, tile.y)];
  }

  Vec2 DistanceField::directionAt(
    const TileMap &map,
    Vec2           point
  ) const {
    const TileCoordinateProps tile = TileMap::tileAt(point);
    if(!map.contains(tile.x, tile.y) || distances_.empty()) return Vec2 {};
    std::uint16_t best_distance = distances_[map.cellIndex(tile.x, tile.y)];
    Vec2          best_direction {};
    for(const TileCoordinateProps &offset : eight_neighbors) {
      const int neighbor_x = tile.x + offset.x;
      const int neighbor_y = tile.y + offset.y;
      if(map.isBlocked(neighbor_x, neighbor_y)) continue;
      const bool is_diagonal = offset.x != 0 && offset.y != 0;
      if(
        is_diagonal && (map.isBlocked(tile.x + offset.x, tile.y) ||
                        map.isBlocked(tile.x, tile.y + offset.y))
      )
        continue;
      const std::uint16_t distance =
        distances_[map.cellIndex(neighbor_x, neighbor_y)];
      if(distance < best_distance) {
        best_distance = distance;
        best_direction =
          TileMap::tileCenter(neighbor_x, neighbor_y).sub(point).normalized();
      }
    }
    return best_direction;
  }
} // namespace wheel_of_fear
