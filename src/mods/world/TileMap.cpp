#include "mods/world/TileMap.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <stdexcept>

#include "core/render/RenderConfig.hpp"

namespace wheel_of_fear {
  namespace {
    constexpr int                                doorway_corner_margin = 4;
    constexpr float                              extra_doorway_chance  = 0.3F;
    constexpr int                                clearing_half_width   = 5;
    constexpr int                                clearing_half_height  = 4;
    constexpr std::array<TileCoordinateProps, 4> four_steps {
      {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}
    };
  } // namespace

  void TileMap::generate(
    RandomGenerator &random,
    int              grid_columns,
    int              grid_rows
  ) {
    const int map_width  = grid_columns * room_cell_width + 1;
    const int map_height = grid_rows * room_cell_height + 1;
    if(
      map_width >
      static_cast<int>(RenderConfig::max_map_size) ||
      map_height > static_cast<int>(RenderConfig::max_map_size)
    ) {
      throw std::runtime_error("map too large");
    }
    const auto cell_count = static_cast<std::size_t>(map_width * map_height);
    width_                = map_width;
    height_               = map_height;
    grid_columns_         = grid_columns;
    grid_rows_            = grid_rows;
    walls_.assign(cell_count, 1);
    structural_.assign(cell_count, 1);
    blocked_.assign(cell_count, 0);
    acid_seconds_.assign(cell_count, 0);
    neon_seconds_.assign(cell_count, 0);
    room_indices_.assign(cell_count, -1);

    carveRooms(random);
    placeDoorways(random);
    openSpanningDoorways(random);
    for(std::size_t room_index = 0; room_index < rooms_.size(); ++room_index)
      furnishRoom(random, room_index);
    markChanged();
  }

  void TileMap::carveRooms(
    RandomGenerator &random
  ) {
    rooms_.clear();
    for(int row = 0; row < grid_rows_; ++row) {
      for(int column = 0; column < grid_columns_; ++column) {
        const RoomProps room {
          column * room_cell_width + 1,
          row * room_cell_height + 1,
          room_cell_width - 1,
          room_cell_height - 1,
          column,
          row,
          all_room_layouts[random.index(all_room_layouts.size())],
        };
        const auto room_index = static_cast<std::int16_t>(rooms_.size());
        rooms_.push_back(room);
        for(int y = room.top; y < room.top + room.height; ++y) {
          for(int x = room.left; x < room.left + room.width; ++x) {
            const std::size_t cell = cellIndex(x, y);
            walls_[cell]           = 0;
            structural_[cell]      = 0;
            room_indices_[cell]    = room_index;
          }
        }
      }
    }
  }

  void TileMap::placeDoorways(
    RandomGenerator &random
  ) {
    doorways_.clear();
    for(int row = 0; row < grid_rows_; ++row) {
      for(int column = 0; column < grid_columns_; ++column) {
        const auto room_index =
          static_cast<std::size_t>(row * grid_columns_ + column);
        const RoomProps room = rooms_[room_index];
        if(column + 1 < grid_columns_) {
          const int offset = random.intInclusive(
            doorway_corner_margin,
            room.height - doorway_corner_margin - DoorwayProps::length
          );
          doorways_.push_back(
            DoorwayProps {
              (column + 1) * room_cell_width,
              room.top + offset,
              true,
              room_index,
              room_index + 1
            }
          );
        }
        if(row + 1 < grid_rows_) {
          const int offset = random.intInclusive(
            doorway_corner_margin,
            room.width - doorway_corner_margin - DoorwayProps::length
          );
          doorways_.push_back(
            DoorwayProps {
              room.left + offset,
              (row + 1) * room_cell_height,
              false,
              room_index,
              room_index + static_cast<std::size_t>(grid_columns_)
            }
          );
        }
      }
    }
  }

  void TileMap::openSpanningDoorways(
    RandomGenerator &random
  ) {
    const std::size_t        room_count = rooms_.size();
    std::vector<bool>        visited(room_count, false);
    std::vector<std::size_t> stack;
    const std::size_t        start = random.index(room_count);
    visited[start]                 = true;
    stack.push_back(start);
    while(!stack.empty()) {
      const std::size_t          current = stack.back();
      std::array<std::size_t, 4> options {};
      std::size_t                option_count = 0;
      for(
        std::size_t doorway_index = 0; doorway_index < doorways_.size();
        ++doorway_index
      ) {
        const DoorwayProps &doorway = doorways_[doorway_index];
        if(!doorway.connects(current) || visited[doorway.otherRoom(current)])
          continue;
        options[option_count++] = doorway_index;
      }
      if(option_count == 0) {
        stack.pop_back();
        continue;
      }
      DoorwayProps &chosen   = doorways_[options[random.index(option_count)]];
      chosen.is_open         = true;
      const std::size_t next = chosen.otherRoom(current);
      visited[next]          = true;
      stack.push_back(next);
    }
    for(DoorwayProps &doorway : doorways_) {
      if(!doorway.is_open && random.chance(extra_doorway_chance))
        doorway.is_open = true;
      doorway.open_amount = doorway.is_open ? 1.0F : 0.0F;
      setDoorwayTiles(doorway, doorway.is_open);
    }
  }

  void TileMap::setDoorwayTiles(
    const DoorwayProps &doorway,
    bool                is_passable
  ) {
    for(int step = 0; step < DoorwayProps::length; ++step) {
      const TileCoordinateProps tile = doorway.tileAt(step);
      const std::size_t         cell = cellIndex(tile.x, tile.y);
      walls_[cell]                   = is_passable ? 0 : 1;
      if(!is_passable) {
        acid_seconds_[cell] = 0;
        neon_seconds_[cell] = 0;
      }
    }
    markChanged();
  }

  bool TileMap::isRoomGraphConnected() const {
    const std::size_t room_count = rooms_.size();
    if(room_count == 0) return true;
    std::vector<bool>        reached(room_count, false);
    std::vector<std::size_t> queue {0};
    reached[0] = true;
    for(std::size_t head = 0; head < queue.size(); ++head) {
      const std::size_t current = queue[head];
      for(const DoorwayProps &doorway : doorways_) {
        if(!doorway.is_open || !doorway.connects(current)) continue;
        const std::size_t next = doorway.otherRoom(current);
        if(reached[next]) continue;
        reached[next] = true;
        queue.push_back(next);
      }
    }
    return queue.size() == room_count;
  }

  bool TileMap::isInClearing(
    const RoomProps &room,
    int              x,
    int              y
  ) {
    return std::abs(x - room.centerTileX()) <=
           clearing_half_width &&
           std::abs(y - room.centerTileY()) <= clearing_half_height;
  }

  bool TileMap::isInsideFurnishableArea(
    const RoomProps &room,
    int              x,
    int              y
  ) {
    return x >=
           room.left +
           free_border &&
           y >=
           room.top +
           free_border &&
           x <
           room.left +
           room.width -
           free_border &&
           y <
           room.top +
           room.height -
           free_border &&
           !isInClearing(room, x, y);
  }

  bool TileMap::placeWallBlock(
    const RoomProps &room,
    int              x,
    int              y,
    int              block_width,
    int              block_height
  ) {
    for(int check_y = y; check_y < y + block_height; ++check_y) {
      for(int check_x = x; check_x < x + block_width; ++check_x)
        if(
          !isInsideFurnishableArea(room, check_x, check_y) ||
          isWall(check_x, check_y)
        )
          return false;
    }
    for(int fill_y = y; fill_y < y + block_height; ++fill_y)
      for(int fill_x = x; fill_x < x + block_width; ++fill_x)
        walls_[cellIndex(fill_x, fill_y)] = 1;
    return true;
  }

  int TileMap::randomInteriorX(
    const RoomProps &room,
    RandomGenerator &random
  ) {
    return random.intInclusive(
      room.left + free_border,
      room.left + room.width - free_border - 1
    );
  }

  int TileMap::randomInteriorY(
    const RoomProps &room,
    RandomGenerator &random
  ) {
    return random.intInclusive(
      room.top + free_border,
      room.top + room.height - free_border - 1
    );
  }

  void TileMap::furnishRoom(
    RandomGenerator &random,
    std::size_t      room_index
  ) {
    const RoomProps room = rooms_[room_index];
    switch(room.layout) {
      case RoomLayoutEnum::hall:
        for(
          int y = room.top + free_border + 1;
          y < room.top + room.height - free_border - 1;
          y += 5
        ) {
          for(
            int x = room.left + free_border + 1;
            x < room.left + room.width - free_border - 1;
            x += 6
          ) {
            if(random.chance(0.75F)) placeWallBlock(room, x, y, 2, 2);
          }
        }
        break;
      case RoomLayoutEnum::offices:
        for(int attempt = 0; attempt < 5; ++attempt) {
          const bool is_horizontal = random.chance(0.5F);
          const int  span          = random.intInclusive(4, 8);
          const int  x             = randomInteriorX(room, random);
          const int  y             = randomInteriorY(room, random);
          placeWallBlock(
            room,
            x,
            y,
            is_horizontal ? span : 1,
            is_horizontal ? 1 : span
          );
        }
        break;
      case RoomLayoutEnum::server_farm:
        for(int attempt = 0; attempt < 2; ++attempt) {
          const int x = randomInteriorX(room, random);
          const int y = randomInteriorY(room, random);
          placeWallBlock(room, x, y, random.intInclusive(3, 5), 1);
        }
        break;
      case RoomLayoutEnum::storage:
        for(int attempt = 0; attempt < 2; ++attempt) {
          const int x = randomInteriorX(room, random);
          const int y = randomInteriorY(room, random);
          if(placeWallBlock(room, x, y, 5, 1))
            placeWallBlock(room, x, y + 1, 1, 3);
        }
        break;
      case RoomLayoutEnum::pump_station:
        for(int attempt = 0; attempt < 6; ++attempt) {
          const int x = randomInteriorX(room, random);
          const int y = randomInteriorY(room, random);
          placeWallBlock(room, x, y, 1, 1);
        }
        break;
    }
    sealUnreachableFloor(room);
  }

  void TileMap::sealUnreachableFloor(
    const RoomProps &room
  ) {
    std::vector<bool> reached(
      static_cast<std::size_t>(room.width * room.height),
      false
    );
    std::vector<TileCoordinateProps> queue {
      {room.left, room.top}
    };
    reached[0] = true;
    for(std::size_t head = 0; head < queue.size(); ++head) {
      const TileCoordinateProps current = queue[head];
      for(const TileCoordinateProps &step : four_steps) {
        const int x = current.x + step.x;
        const int y = current.y + step.y;
        if(!room.containsTile(x, y) || isWall(x, y)) continue;
        const auto local = static_cast<std::size_t>(
          (y - room.top) * room.width + (x - room.left)
        );
        if(reached[local]) continue;
        reached[local] = true;
        queue.push_back({x, y});
      }
    }
    for(int y = room.top; y < room.top + room.height; ++y) {
      for(int x = room.left; x < room.left + room.width; ++x) {
        const auto local = static_cast<std::size_t>(
          (y - room.top) * room.width + (x - room.left)
        );
        if(!reached[local]) walls_[cellIndex(x, y)] = 1;
      }
    }
  }

  bool TileMap::isNearDoorway(
    int x,
    int y,
    int margin
  ) const {
    for(const DoorwayProps &doorway : doorways_) {
      for(int step = 0; step < DoorwayProps::length; ++step) {
        const TileCoordinateProps tile = doorway.tileAt(step);
        if(std::abs(tile.x - x) <= margin && std::abs(tile.y - y) <= margin)
          return true;
      }
    }
    return false;
  }

  bool TileMap::isAreaFree(
    int x,
    int y,
    int area_width,
    int area_height
  ) const {
    for(int check_y = y; check_y < y + area_height; ++check_y) {
      for(int check_x = x; check_x < x + area_width; ++check_x)
        if(isBlocked(check_x, check_y) || !roomIndexAtTile(check_x, check_y))
          return false;
    }
    return true;
  }

  bool TileMap::reserveProp(
    int x,
    int y,
    int area_width,
    int area_height
  ) {
    if(!isAreaFree(x, y, area_width, area_height)) return false;
    for(int check_y = y; check_y < y + area_height; ++check_y) {
      for(int check_x = x; check_x < x + area_width; ++check_x)
        if(isNearDoorway(check_x, check_y, 2)) return false;
    }
    for(int fill_y = y; fill_y < y + area_height; ++fill_y)
      for(int fill_x = x; fill_x < x + area_width; ++fill_x)
        blocked_[cellIndex(fill_x, fill_y)] = 1;
    markChanged();
    return true;
  }

  void TileMap::releaseProp(
    int x,
    int y,
    int area_width,
    int area_height
  ) {
    for(int fill_y = y; fill_y < y + area_height; ++fill_y) {
      for(int fill_x = x; fill_x < x + area_width; ++fill_x)
        if(contains(fill_x, fill_y)) blocked_[cellIndex(fill_x, fill_y)] = 0;
    }
    markChanged();
  }

  bool TileMap::isRoomOpenAreaConnected(
    const RoomProps &room
  ) const {
    std::vector<bool> reached(
      static_cast<std::size_t>(room.width * room.height),
      false
    );
    std::vector<TileCoordinateProps> queue;
    std::size_t                      open_count = 0;
    for(int y = room.top; y < room.top + room.height; ++y) {
      for(int x = room.left; x < room.left + room.width; ++x) {
        if(isBlocked(x, y)) continue;
        open_count += 1;
        if(queue.empty()) {
          reached[static_cast<std::size_t>(
            (y - room.top) * room.width + (x - room.left)
          )] = true;
          queue.push_back({x, y});
        }
      }
    }
    for(std::size_t head = 0; head < queue.size(); ++head) {
      const TileCoordinateProps current = queue[head];
      for(const TileCoordinateProps &step : four_steps) {
        const int x      = current.x + step.x;
        const int next_y = current.y + step.y;
        if(!room.containsTile(x, next_y) || isBlocked(x, next_y)) continue;
        const auto local = static_cast<std::size_t>(
          (next_y - room.top) * room.width + (x - room.left)
        );
        if(reached[local]) continue;
        reached[local] = true;
        queue.push_back({x, next_y});
      }
    }
    return queue.size() == open_count;
  }

  bool TileMap::isEveryFloorConnected() const {
    if(rooms_.empty()) return true;
    std::vector<bool> reached(walls_.size(), false);
    const auto        start = nearestOpenTile(tileAt(rooms_[0].center()));
    if(!start) return false;
    const std::size_t start_index = cellIndex(start->x, start->y);
    reached[start_index]          = true;
    std::vector<std::uint32_t> queue {static_cast<std::uint32_t>(start_index)};
    const auto                 map_width = static_cast<std::uint32_t>(width_);
    for(std::size_t head = 0; head < queue.size(); ++head) {
      const std::uint32_t cell = queue[head];
      const auto          x    = static_cast<int>(cell % map_width);
      const auto          y    = static_cast<int>(cell / map_width);
      for(const TileCoordinateProps &step : four_steps) {
        if(isBlocked(x + step.x, y + step.y)) continue;
        const std::size_t neighbor = cellIndex(x + step.x, y + step.y);
        if(reached[neighbor]) continue;
        reached[neighbor] = true;
        queue.push_back(static_cast<std::uint32_t>(neighbor));
      }
    }
    std::size_t open_count = 0;
    for(std::size_t cell = 0; cell < walls_.size(); ++cell)
      if(walls_[cell] == 0 && blocked_[cell] == 0) open_count += 1;
    return open_count == queue.size();
  }

  std::optional<TileCoordinateProps> TileMap::nearestOpenTile(
    TileCoordinateProps origin
  ) const {
    if(!isBlocked(origin.x, origin.y)) return origin;
    for(int radius = 1; radius <= 8; ++radius) {
      for(int y = origin.y - radius; y <= origin.y + radius; ++y) {
        for(int x = origin.x - radius; x <= origin.x + radius; ++x)
          if(!isBlocked(x, y)) return TileCoordinateProps {x, y};
      }
    }
    return std::nullopt;
  }

  Vec2 TileMap::nearestFloor(
    Vec2 point
  ) const {
    const TileCoordinateProps origin = tileAt(point);
    if(!isBlocked(origin.x, origin.y)) return point;
    for(int radius = 1; radius <= 8; ++radius) {
      std::optional<Vec2> best;
      float best_distance = std::numeric_limits<float>::infinity();
      for(int y = origin.y - radius; y <= origin.y + radius; ++y) {
        for(int x = origin.x - radius; x <= origin.x + radius; ++x) {
          if(isBlocked(x, y)) continue;
          const Vec2  candidate = tileCenter(x, y);
          const float distance  = candidate.distance(point);
          if(distance < best_distance) {
            best_distance = distance;
            best          = candidate;
          }
        }
      }
      if(best) return *best;
    }
    return point;
  }

  bool TileMap::isInterior(
    int x,
    int y
  ) const {
    return x >= 1 && y >= 1 && x < width_ - 1 && y < height_ - 1;
  }

  bool TileMap::contains(
    int x,
    int y
  ) const {
    return x >= 0 && y >= 0 && x < width_ && y < height_;
  }

  std::size_t TileMap::cellIndex(
    int x,
    int y
  ) const {
    return static_cast<std::size_t>(y * width_ + x);
  }

  bool TileMap::isWall(
    int x,
    int y
  ) const {
    if(!contains(x, y)) return true;
    return walls_[cellIndex(x, y)] != 0;
  }

  bool TileMap::isStructural(
    int x,
    int y
  ) const {
    if(!contains(x, y)) return true;
    return structural_[cellIndex(x, y)] != 0;
  }

  bool TileMap::isBlocked(
    int x,
    int y
  ) const {
    if(!contains(x, y)) return true;
    const std::size_t cell = cellIndex(x, y);
    return walls_[cell] != 0 || blocked_[cell] != 0;
  }

  TileCoordinateProps TileMap::tileAt(
    Vec2 point
  ) {
    return TileCoordinateProps {
      static_cast<int>(std::floor(point.x / RenderConfig::tile_size)),
      static_cast<int>(std::floor(point.y / RenderConfig::tile_size))
    };
  }

  Vec2 TileMap::tileCenter(
    int x,
    int y
  ) {
    return Vec2::of(
      (static_cast<float>(x) + 0.5F) * RenderConfig::tile_size,
      (static_cast<float>(y) + 0.5F) * RenderConfig::tile_size
    );
  }

  bool TileMap::isSolidAt(
    Vec2 point
  ) const {
    const TileCoordinateProps tile = tileAt(point);
    return isBlocked(tile.x, tile.y);
  }

  bool TileMap::isWallAt(
    Vec2 point
  ) const {
    const TileCoordinateProps tile = tileAt(point);
    return isWall(tile.x, tile.y);
  }

  float TileMap::acidAt(
    Vec2 point
  ) const {
    const TileCoordinateProps tile = tileAt(point);
    if(!contains(tile.x, tile.y)) return 0;
    return acid_seconds_[cellIndex(tile.x, tile.y)];
  }

  void TileMap::spillAcid(
    Vec2  point,
    float seconds
  ) {
    const TileCoordinateProps tile = tileAt(point);
    if(!contains(tile.x, tile.y) || isWall(tile.x, tile.y)) return;
    float &cell = acid_seconds_[cellIndex(tile.x, tile.y)];
    cell        = std::max(cell, seconds);
  }

  float TileMap::neonAt(
    Vec2 point
  ) const {
    const TileCoordinateProps tile = tileAt(point);
    return neonAtTile(tile.x, tile.y);
  }

  float TileMap::neonAtTile(
    int x,
    int y
  ) const {
    if(!contains(x, y)) return 0;
    return neon_seconds_[cellIndex(x, y)];
  }

  void TileMap::spillNeon(
    int   x,
    int   y,
    float seconds
  ) {
    if(!contains(x, y) || isWall(x, y)) return;
    float &cell = neon_seconds_[cellIndex(x, y)];
    cell        = std::max(cell, seconds);
  }

  bool TileMap::dissolveWall(
    int x,
    int y
  ) {
    if(!isInterior(x, y) || !isWall(x, y)) return false;
    const std::size_t cell = cellIndex(x, y);
    if(structural_[cell] != 0) return false;
    walls_[cell] = 0;
    markChanged();
    return true;
  }

  void TileMap::evaporate(
    float seconds
  ) {
    for(float &acid : acid_seconds_)
      if(acid > 0) acid = std::max(0.0F, acid - seconds);
    for(float &neon : neon_seconds_)
      if(neon > 0) neon = std::max(0.0F, neon - seconds);
  }

  bool TileMap::collidesWithBox(
    Vec2  center,
    float half_size
  ) const {
    return isSolidAt(Vec2::of(center.x - half_size, center.y - half_size)) ||
           isSolidAt(Vec2::of(center.x + half_size, center.y - half_size)) ||
           isSolidAt(Vec2::of(center.x - half_size, center.y + half_size)) ||
           isSolidAt(Vec2::of(center.x + half_size, center.y + half_size));
  }

  Vec2 TileMap::slide(
    Vec2  position,
    Vec2  displacement,
    float half_size
  ) const {
    Vec2       current = position;
    const auto step_count =
      static_cast<std::size_t>(std::ceil(displacement.length() / 4.0F) + 1);
    const Vec2 step = displacement.scale(1.0F / static_cast<float>(step_count));
    for(std::size_t index = 0; index < step_count; ++index) {
      const Vec2 horizontal = Vec2::of(current.x + step.x, current.y);
      if(!collidesWithBox(horizontal, half_size)) current = horizontal;
      const Vec2 vertical = Vec2::of(current.x, current.y + step.y);
      if(!collidesWithBox(vertical, half_size)) current = vertical;
    }
    return current;
  }

  bool TileMap::hasLineOfSight(
    Vec2 from,
    Vec2 to
  ) const {
    const auto step_count =
      static_cast<std::size_t>(std::max(to.distance(from) / 5.0F, 1.0F));
    const TileCoordinateProps destination_tile = tileAt(to);
    for(std::size_t step = 1; step < step_count; ++step) {
      const TileCoordinateProps tile = tileAt(
        from.lerp(to, static_cast<float>(step) / static_cast<float>(step_count))
      );
      if(tile == destination_tile) return true;
      if(isWall(tile.x, tile.y)) return false;
    }
    return true;
  }

  Vec2 TileMap::randomFloorInRoom(
    RandomGenerator &random,
    const RoomProps &room
  ) const {
    for(int attempt = 0; attempt < 60; ++attempt) {
      const int x =
        random.intInclusive(room.left + 1, room.left + room.width - 2);
      const int y =
        random.intInclusive(room.top + 1, room.top + room.height - 2);
      if(
        !isBlocked(x, y) &&
        !isBlocked(x + 1, y) &&
        !isBlocked(x - 1, y) &&
        !isBlocked(x, y + 1) &&
        !isBlocked(x, y - 1)
      )
        return tileCenter(x, y);
    }
    return nearestFloor(room.center());
  }

  std::optional<std::size_t> TileMap::roomIndexAtTile(
    int x,
    int y
  ) const {
    if(!contains(x, y)) return std::nullopt;
    const std::int16_t room_index = room_indices_[cellIndex(x, y)];
    if(room_index < 0) return std::nullopt;
    return static_cast<std::size_t>(room_index);
  }

  std::optional<std::size_t> TileMap::roomIndexAt(
    Vec2 point
  ) const {
    const TileCoordinateProps tile = tileAt(point);
    return roomIndexAtTile(tile.x, tile.y);
  }

  int TileMap::roomGridDistance(
    std::size_t first_room,
    std::size_t second_room
  ) const {
    return rooms_[first_room].gridDistance(rooms_[second_room]);
  }
} // namespace wheel_of_fear
