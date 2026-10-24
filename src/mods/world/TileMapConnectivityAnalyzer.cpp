#include "mods/world/TileMapConnectivityAnalyzer.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace wheel_of_fear {
  bool
  TileMapConnectivityAnalyzer::isRoomGraphConnected(const TileMapModel &map) {
    const std::size_t room_count = map.rooms().size();
    if(room_count == 0) return true;

    std::vector<bool>        reached(room_count, false);
    std::vector<std::size_t> queue {0};
    reached[0] = true;

    for(std::size_t head = 0; head < queue.size(); ++head) {
      const std::size_t current = queue[head];

      for(const DoorwayProps &doorway : map.doorways()) {
        if(!doorway.is_open || !doorway.connects(current)) continue;

        const std::size_t next = doorway.otherRoom(current);
        if(reached[next]) continue;

        reached[next] = true;
        queue.push_back(next);
      }
    }

    return queue.size() == room_count;
  }

  bool TileMapConnectivityAnalyzer::isRoomOpenAreaConnected(
    const TileMapModel &map,
    const RoomProps    &room
  ) {
    std::vector<bool>                reached(room.tileCount(), false);
    std::vector<TileCoordinateProps> queue;
    std::size_t                      open_count = 0;

    for(int y = room.top; y < room.top + room.height; ++y) {
      for(int x = room.left; x < room.left + room.width; ++x) {
        if(map.isBlocked(x, y)) continue;
        open_count += 1;

        if(queue.empty()) {
          reached[room.localIndex(x, y)] = true;
          queue.push_back({.x = x, .y = y});
        }
      }
    }

    for(std::size_t head = 0; head < queue.size(); ++head) {
      const TileCoordinateProps current = queue[head];

      for(const TileCoordinateProps &step : four_neighbor_steps) {
        const int x = current.x + step.x;
        const int y = current.y + step.y;
        if(!room.containsTile(x, y) || map.isBlocked(x, y)) continue;

        const auto local = room.localIndex(x, y);
        if(reached[local]) continue;

        reached[local] = true;
        queue.push_back({.x = x, .y = y});
      }
    }

    return queue.size() == open_count;
  }

  bool
  TileMapConnectivityAnalyzer::isEveryFloorConnected(const TileMapModel &map) {
    if(map.rooms().empty()) return true;
    const auto cell_count = static_cast<std::size_t>(map.width())
      * static_cast<std::size_t>(map.height());

    std::vector<bool> reached(cell_count, false);
    const auto        start
      = map.nearestOpenTile(TileMapModel::tileAt(map.rooms()[0].center()));

    if(!start) return false;
    const std::size_t start_index = map.cellIndex(start->x, start->y);

    reached[start_index] = true;

    std::vector<std::uint32_t> queue {static_cast<std::uint32_t>(start_index)};
    const auto map_width = static_cast<std::uint32_t>(map.width());

    for(std::size_t head = 0; head < queue.size(); ++head) {
      const std::uint32_t cell = queue[head];
      const auto          x    = static_cast<int>(cell % map_width);
      const auto          y    = static_cast<int>(cell / map_width);

      for(const TileCoordinateProps &step : four_neighbor_steps) {
        if(map.isBlocked(x + step.x, y + step.y)) continue;

        const std::size_t neighbor = map.cellIndex(x + step.x, y + step.y);
        if(reached[neighbor]) continue;

        reached[neighbor] = true;
        queue.push_back(static_cast<std::uint32_t>(neighbor));
      }
    }

    std::size_t open_count = 0;

    for(int y = 0; y < map.height(); ++y)
      for(int x = 0; x < map.width(); ++x)
        if(!map.isBlocked(x, y)) open_count += 1;

    return open_count == queue.size();
  }
} // namespace wheel_of_fear
