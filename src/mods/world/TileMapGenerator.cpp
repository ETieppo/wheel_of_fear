#include "mods/world/TileMapGenerator.hpp"
#include "core/render/RenderConfig.hpp"
#include "mods/world/RoomShaper.hpp"
#include <array>
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace wheel_of_fear {
  void TileMapGenerator::generate(
    int grid_columns,
    int grid_rows
  ) {
    const int map_width  = (grid_columns * room_cell_width) + 1;
    const int map_height = (grid_rows * room_cell_height) + 1;

    if(
      std::cmp_greater(map_width, RenderConfig::max_map_size)
      || std::cmp_greater(map_height, RenderConfig::max_map_size)
    ) {
      throw std::runtime_error("map too large");
    }

    grid_columns_ = grid_columns;
    grid_rows_    = grid_rows;
    map_.reset(map_width, map_height);
    carveRooms();
    placeDoorways();
    openSpanningDoorways();
    shapeRooms();

    for(
      std::size_t room_index = 0; room_index < map_.rooms().size(); ++room_index
    )
      furnishRoom(room_index);

    map_.markChanged();
  }

  void TileMapGenerator::carveRooms() {
    std::vector<RoomProps> rooms;

    for(int row = 0; row < grid_rows_; ++row) {
      for(int column = 0; column < grid_columns_; ++column) {
        const RoomProps room {
          .left   = (column * room_cell_width) + 1,
          .top    = (row * room_cell_height) + 1,
          .width  = room_cell_width - 1,
          .height = room_cell_height - 1,
          .column = column,
          .row    = row,
          .layout = all_room_layouts[random_.index(all_room_layouts.size())],
        };

        const std::size_t room_index = rooms.size();
        rooms.push_back(room);

        for(int y = room.top; y < room.top + room.height; ++y)
          for(int x = room.left; x < room.left + room.width; ++x)
            map_.carveFloor(x, y, room_index);
      }
    }

    map_.setRooms(std::move(rooms));
  }

  void TileMapGenerator::shapeRooms() {
    RoomShaper shaper(map_, random_);

    for(
      std::size_t room_index = 0; room_index < map_.rooms().size(); ++room_index
    )
      shaper.shape(room_index);
  }

  void TileMapGenerator::placeDoorways() {
    std::vector<DoorwayProps> &doorways = map_.doorways();
    doorways.clear();

    for(int row = 0; row < grid_rows_; ++row) {
      for(int column = 0; column < grid_columns_; ++column) {
        const auto room_index = (static_cast<std::size_t>(row)
                                 * static_cast<std::size_t>(grid_columns_))
          + static_cast<std::size_t>(column);
        const RoomProps room = map_.rooms()[room_index];

        if(column + 1 < grid_columns_) {
          const int offset = random_.intInclusive(
            doorway_corner_margin,
            room.height - doorway_corner_margin - DoorwayProps::length
          );

          doorways.push_back(
            DoorwayProps {
              .first_x     = (column + 1) * room_cell_width,
              .first_y     = room.top + offset,
              .is_vertical = true,
              .room_a      = room_index,
              .room_b      = room_index + 1,
            }
          );
        }

        if(row + 1 < grid_rows_) {
          const int offset = random_.intInclusive(
            doorway_corner_margin,
            room.width - doorway_corner_margin - DoorwayProps::length
          );

          doorways.push_back(
            DoorwayProps {
              .first_x     = room.left + offset,
              .first_y     = (row + 1) * room_cell_height,
              .is_vertical = false,
              .room_a      = room_index,
              .room_b = room_index + static_cast<std::size_t>(grid_columns_),
            }
          );
        }
      }
    }
  }

  void TileMapGenerator::openSpanningDoorways() {
    std::vector<DoorwayProps> &doorways   = map_.doorways();
    const std::size_t          room_count = map_.rooms().size();

    std::vector<bool>        visited(room_count, false);
    std::vector<std::size_t> stack;
    const std::size_t        start = random_.index(room_count);

    visited[start] = true;
    stack.push_back(start);

    while(!stack.empty()) {
      const std::size_t          current = stack.back();
      std::array<std::size_t, 4> options {};
      std::size_t                option_count = 0;

      for(
        std::size_t doorway_index = 0; doorway_index < doorways.size();
        ++doorway_index
      ) {
        const DoorwayProps &doorway = doorways[doorway_index];
        if(!doorway.connects(current) || visited[doorway.otherRoom(current)])
          continue;

        options[option_count++] = doorway_index;
      }

      if(option_count == 0) {
        stack.pop_back();
        continue;
      }

      DoorwayProps &chosen   = doorways[options[random_.index(option_count)]];
      chosen.is_open         = true;
      const std::size_t next = chosen.otherRoom(current);

      visited[next] = true;
      stack.push_back(next);
    }

    for(DoorwayProps &doorway : doorways) {
      if(!doorway.is_open && random_.chance(extra_doorway_chance))
        doorway.is_open = true;
      doorway.open_amount = doorway.is_open ? 1.0F : 0.0F;
      map_.setDoorwayTiles(doorway, doorway.is_open);
    }
  }

  bool TileMapGenerator::isInClearing(
    const RoomProps &room,
    int              x,
    int              y
  ) {
    return std::abs(x - room.centerTileX())
      <= clearing_half_width
      && std::abs(y - room.centerTileY())
      <= clearing_half_height;
  }

  bool TileMapGenerator::isInsideFurnishableArea(
    std::size_t room_index,
    int         x,
    int         y
  ) const {
    const RoomProps &room = map_.rooms()[room_index];
    if(isInClearing(room, x, y)) return false;

    for(
      int offset_y = -RoomProps::free_border;
      offset_y <= RoomProps::free_border;
      ++offset_y
    )
      for(
        int offset_x = -RoomProps::free_border;
        offset_x <= RoomProps::free_border;
        ++offset_x
      )
        if(map_.roomIndexAtTile(x + offset_x, y + offset_y) != room_index)
          return false;

    return true;
  }

  bool TileMapGenerator::placeWallBlock(
    std::size_t          room_index,
    const TileAreaProps &block
  ) {
    for(
      int check_y = block.top; check_y < block.top + block.height; ++check_y
    ) {
      for(
        int check_x = block.left; check_x < block.left + block.width; ++check_x
      )
        if(
          !isInsideFurnishableArea(room_index, check_x, check_y)
          || map_.isWall(check_x, check_y)
        )
          return false;
    }

    for(int fill_y = block.top; fill_y < block.top + block.height; ++fill_y)
      for(int fill_x = block.left; fill_x < block.left + block.width; ++fill_x)
        map_.raiseWall(fill_x, fill_y);

    return true;
  }

  int TileMapGenerator::randomInteriorX(const RoomProps &room) const {
    return random_.intInclusive(
      room.left + RoomProps::free_border,
      room.left + room.width - RoomProps::free_border - 1
    );
  }

  int TileMapGenerator::randomInteriorY(const RoomProps &room) const {
    return random_.intInclusive(
      room.top + RoomProps::free_border,
      room.top + room.height - RoomProps::free_border - 1
    );
  }

  void TileMapGenerator::furnishRoom(std::size_t room_index) {
    const RoomProps room = map_.rooms()[room_index];

    switch(room.layout) {
      case RoomLayoutEnum::hall:         furnishHall(room_index); break;
      case RoomLayoutEnum::offices:      furnishOffices(room_index); break;
      case RoomLayoutEnum::server_farm:  furnishServerFarm(room_index); break;
      case RoomLayoutEnum::storage:      furnishStorage(room_index); break;
      case RoomLayoutEnum::pump_station: furnishPumpStation(room_index); break;
    }

    sealUnreachableFloor(room_index);
  }

  void TileMapGenerator::furnishHall(std::size_t room_index) {
    const RoomProps &room = map_.rooms()[room_index];

    for(
      int y = room.top + RoomProps::free_border + 1;
      y < room.top + room.height - RoomProps::free_border - 1;
      y += 5
    ) {
      for(
        int x = room.left + RoomProps::free_border + 1;
        x < room.left + room.width - RoomProps::free_border - 1;
        x += 6
      ) {
        if(random_.chance(0.75F))
          placeWallBlock(
            room_index,
            TileAreaProps {.left = x, .top = y, .width = 2, .height = 2}
          );
      }
    }
  }

  void TileMapGenerator::furnishOffices(std::size_t room_index) {
    const RoomProps &room = map_.rooms()[room_index];

    for(int attempt = 0; attempt < 5; ++attempt) {
      const bool is_horizontal = random_.chance(0.5F);
      const int  span          = random_.intInclusive(4, 8);
      const int  x             = randomInteriorX(room);
      const int  y             = randomInteriorY(room);

      placeWallBlock(
        room_index,
        TileAreaProps {
          .left   = x,
          .top    = y,
          .width  = is_horizontal ? span : 1,
          .height = is_horizontal ? 1 : span,
        }
      );
    }
  }

  void TileMapGenerator::furnishServerFarm(std::size_t room_index) {
    const RoomProps &room = map_.rooms()[room_index];

    for(int attempt = 0; attempt < 2; ++attempt) {
      const int x = randomInteriorX(room);
      const int y = randomInteriorY(room);

      placeWallBlock(
        room_index,
        TileAreaProps {
          .left   = x,
          .top    = y,
          .width  = random_.intInclusive(3, 5),
          .height = 1,
        }
      );
    }
  }

  void TileMapGenerator::furnishStorage(std::size_t room_index) {
    const RoomProps &room = map_.rooms()[room_index];

    for(int attempt = 0; attempt < 2; ++attempt) {
      const int           x = randomInteriorX(room);
      const int           y = randomInteriorY(room);
      const TileAreaProps shelf {.left = x, .top = y, .width = 5, .height = 1};
      const TileAreaProps
        post {.left = x, .top = y + 1, .width = 1, .height = 3};

      if(placeWallBlock(room_index, shelf)) placeWallBlock(room_index, post);
    }
  }

  void TileMapGenerator::furnishPumpStation(std::size_t room_index) {
    const RoomProps &room = map_.rooms()[room_index];

    for(int attempt = 0; attempt < 6; ++attempt) {
      const int x = randomInteriorX(room);
      const int y = randomInteriorY(room);

      placeWallBlock(
        room_index,
        TileAreaProps {.left = x, .top = y, .width = 1, .height = 1}
      );
    }
  }

  void TileMapGenerator::sealUnreachableFloor(std::size_t room_index) {
    const RoomProps &room  = map_.rooms()[room_index];
    const auto       start = firstFloorTile(room_index);
    if(!start) return;

    std::vector<bool>                reached(room.tileCount(), false);
    std::vector<TileCoordinateProps> queue {*start};
    reached[room.localIndex(start->x, start->y)] = true;

    for(std::size_t head = 0; head < queue.size(); ++head) {
      const TileCoordinateProps current = queue[head];

      for(const TileCoordinateProps &step : four_neighbor_steps) {
        const int x = current.x + step.x;
        const int y = current.y + step.y;
        if(!room.containsTile(x, y) || map_.isWall(x, y)) continue;
        if(reached[room.localIndex(x, y)]) continue;

        reached[room.localIndex(x, y)] = true;
        queue.push_back({.x = x, .y = y});
      }
    }

    for(int y = room.top; y < room.top + room.height; ++y)
      for(int x = room.left; x < room.left + room.width; ++x)
        if(!reached[room.localIndex(x, y)]) map_.raiseWall(x, y);
  }

  std::optional<TileCoordinateProps>
  TileMapGenerator::firstFloorTile(std::size_t room_index) const {
    const RoomProps &room = map_.rooms()[room_index];

    for(int y = room.top; y < room.top + room.height; ++y)
      for(int x = room.left; x < room.left + room.width; ++x)
        if(map_.roomIndexAtTile(x, y) == room_index && !map_.isWall(x, y))
          return TileCoordinateProps {.x = x, .y = y};

    return std::nullopt;
  }
} // namespace wheel_of_fear
