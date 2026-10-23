#include "mods/world/WorldFurnisher.hpp"

#include <array>
#include <cstdlib>

#include "core/theme/Palette.hpp"
#include "mods/world/World.hpp"
#include "mods/world/scenery/SceneryFactory.hpp"

namespace wheel_of_fear {
  int WorldFurnisher::interiorX(const RoomProps &room) {
    return world_.random().intInclusive(
      room.left + TileMap::free_border,
      room.left + room.width - TileMap::free_border - 1
    );
  }

  int WorldFurnisher::interiorY(const RoomProps &room) {
    return world_.random().intInclusive(
      room.top + TileMap::free_border,
      room.top + room.height - TileMap::free_border - 1
    );
  }

  bool WorldFurnisher::isInClearing(
    const RoomProps &room,
    int              x,
    int              y
  ) {
    return std::abs(x - room.centerTileX()) <=
           4 &&
           std::abs(y - room.centerTileY()) <= 3;
  }

  bool WorldFurnisher::tryReserveScenery(
    const RoomProps &room,
    std::size_t      room_index,
    int              x,
    int              y,
    SceneryKindEnum  kind,
    Color            color
  ) {
    TileMap &map = world_.map();
    if(world_.isNearLandmark(x, y)) return false;
    if(!map.reserveProp(x, y, 1, 1)) return false;
    if(!map.isRoomOpenAreaConnected(room)) {
      map.releaseProp(x, y, 1, 1);
      return false;
    }
    RandomGenerator    &random = world_.random();
    SceneryPlacementDto placement;
    placement.position     = TileMap::tileCenter(x, y);
    placement.mirrored     = random.chance(0.5F);
    placement.room_index   = room_index;
    placement.color        = color;
    placement.flicker_seed = random.unit();
    world_.addScenery(SceneryFactory::create(kind, placement));
    return true;
  }

  void WorldFurnisher::furnishRoom(
    const RoomProps &room,
    std::size_t      room_index,
    Color            color
  ) {
    RandomGenerator &random = world_.random();
    switch(room.layout) {
      case RoomLayoutEnum::server_farm:
        for(int y = room.top + TileMap::free_border;
            y < room.top + room.height - TileMap::free_border;
            y += 3) {
          for(int x = room.left + TileMap::free_border;
              x < room.left + room.width - TileMap::free_border;
              ++x) {
            if((x - room.left) % 7 == 0 || isInClearing(room, x, y)) continue;
            tryReserveScenery(
              room, room_index, x, y, SceneryKindEnum::server_rack, color
            );
          }
        }
        break;
      case RoomLayoutEnum::storage:
        for(int attempt = 0; attempt < 5; ++attempt) {
          const int x = interiorX(room);
          const int y = interiorY(room);
          if(isInClearing(room, x, y)) continue;
          const std::size_t pile_size = random.index(4) + 2;
          for(std::size_t piece = 0; piece < pile_size; ++piece) {
            const SceneryKindEnum kind     = random.chance(0.6F)
                                             ? SceneryKindEnum::crate
                                             : SceneryKindEnum::barrel;
            const int             offset_x = random.intInclusive(-1, 1);
            const int             offset_y = random.intInclusive(-1, 1);
            tryReserveScenery(
              room, room_index, x + offset_x, y + offset_y, kind, color
            );
          }
        }
        break;
      case RoomLayoutEnum::offices:
        for(int attempt = 0; attempt < 6; ++attempt) {
          const int x = interiorX(room);
          const int y = interiorY(room);
          if(!isInClearing(room, x, y))
            tryReserveScenery(
              room, room_index, x, y, SceneryKindEnum::terminal, color
            );
        }
        break;
      case RoomLayoutEnum::pump_station:
        for(int attempt = 0; attempt < 7; ++attempt) {
          const int x = interiorX(room);
          const int y = interiorY(room);
          if(isInClearing(room, x, y)) continue;
          SceneryKindEnum kind = SceneryKindEnum::junction_box;
          if(random.chance(0.5F)) kind = SceneryKindEnum::pipes;
          else if(random.chance(0.5F)) kind = SceneryKindEnum::barrel;
          tryReserveScenery(room, room_index, x, y, kind, color);
        }
        break;
      case RoomLayoutEnum::hall:
        for(int attempt = 0; attempt < 3; ++attempt) {
          const int x = interiorX(room);
          const int y = interiorY(room);
          if(isInClearing(room, x, y)) continue;
          const SceneryKindEnum kind = random.chance(0.5F)
                                       ? SceneryKindEnum::junction_box
                                       : SceneryKindEnum::barrel;
          tryReserveScenery(room, room_index, x, y, kind, color);
        }
        break;
    }
  }

  void WorldFurnisher::placeLockers(
    const RoomProps &room,
    std::size_t      room_index
  ) {
    if(room_index == world_.exitRoomIndex()) return;
    RandomGenerator  &random = world_.random();
    TileMap          &map    = world_.map();
    const std::size_t locker_count =
      room_index == world_.startRoomIndex() ? 1 : random.index(2) + 1;
    std::size_t placed = 0;
    for(int attempt = 0; placed < locker_count && attempt < 30; ++attempt) {
      const bool faces_down = random.chance(0.5F);
      const int  x =
        random.intInclusive(room.left + 2, room.left + room.width - 3);
      const int y = faces_down ? room.top : room.top + room.height - 1;
      if(world_.isNearLandmark(x, y) || map.isWall(x, y)) continue;
      if(!map.reserveProp(x, y, 1, 1)) continue;
      if(!map.isRoomOpenAreaConnected(room)) {
        map.releaseProp(x, y, 1, 1);
        continue;
      }
      world_.addLocker(
        Locker(TileMap::tileCenter(x, y), room_index, faces_down)
      );
      placed += 1;
    }
  }

  void WorldFurnisher::placeNeonSigns(
    const RoomProps &room,
    std::size_t      room_index
  ) {
    constexpr std::array<SceneryKindEnum, 3> kinds {
      SceneryKindEnum::neon_sign_arrow,
      SceneryKindEnum::neon_sign_eye,
      SceneryKindEnum::neon_sign_bars
    };
    RandomGenerator  &random     = world_.random();
    const std::size_t sign_count = random.index(2) + 1;
    for(std::size_t sign = 0; sign < sign_count; ++sign) {
      const int x =
        random.intInclusive(room.left + 3, room.left + room.width - 4);
      if(!world_.map().isWall(x, room.top - 1) ||
         world_.isNearLandmark(x, room.top))
        continue;
      SceneryPlacementDto placement;
      placement.position =
        TileMap::tileCenter(x, room.top).sub(Vec2::of(0, 12));
      const SceneryKindEnum kind = kinds[random.index(kinds.size())];
      placement.mirrored         = random.chance(0.5F);
      placement.room_index       = room_index;
      placement.color =
        Palette::room_neon_colors[random
                                    .index(Palette::room_neon_colors.size())];
      placement.flicker_seed = random.unit();
      world_.addScenery(SceneryFactory::create(kind, placement));
    }
  }
} // namespace wheel_of_fear
