#include "mods/world/WorldFurnisher.hpp"
#include "core/theme/Palette.hpp"
#include "mods/world/TileMapConnectivityAnalyzer.hpp"
#include "mods/world/WorldSystem.hpp"
#include "mods/world/scenery/SceneryFactory.hpp"
#include <array>
#include <cstdlib>

namespace wheel_of_fear {
  int WorldFurnisher::interiorX(const RoomProps &room) {
    return world_.random().intInclusive(
      room.left + RoomProps::free_border,
      room.left + room.width - RoomProps::free_border - 1
    );
  }

  int WorldFurnisher::interiorY(const RoomProps &room) {
    return world_.random().intInclusive(
      room.top + RoomProps::free_border,
      room.top + room.height - RoomProps::free_border - 1
    );
  }

  bool WorldFurnisher::isInClearing(
    const RoomProps &room,
    int              x,
    int              y
  ) {
    return std::abs(x - room.centerTileX())
      <= 4
      && std::abs(y - room.centerTileY())
      <= 3;
  }

  bool WorldFurnisher::tryReserveScenery(
    const RoomProps &room,
    std::size_t      room_index,
    int              x,
    int              y,
    SceneryKindEnum  kind,
    ColorProps       color
  ) {
    TileMapModel &map = world_.map();
    if(world_.isNearLandmark(x, y)) return false;
    if(!map.reserveProp(x, y, 1, 1)) return false;
    if(!TileMapConnectivityAnalyzer::isRoomOpenAreaConnected(map, room)) {
      map.releaseProp(x, y, 1, 1);
      return false;
    }

    RandomGenerator    &random = world_.random();
    SceneryPlacementDto placement;
    placement.position     = TileMapModel::tileCenter(x, y);
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
    ColorProps       color
  ) {
    switch(room.layout) {
      case RoomLayoutEnum::server_farm:
        furnishServerFarm(room, room_index, color);
        break;

      case RoomLayoutEnum::storage:
        furnishStorage(room, room_index, color);
        break;

      case RoomLayoutEnum::offices:
        furnishOffices(room, room_index, color);
        break;

      case RoomLayoutEnum::pump_station:
        furnishPumpStation(room, room_index, color);
        break;

      case RoomLayoutEnum::hall: furnishHall(room, room_index, color); break;
    }
  }

  void WorldFurnisher::furnishServerFarm(
    const RoomProps &room,
    std::size_t      room_index,
    ColorProps       color
  ) {
    for(
      int y = room.top + RoomProps::free_border;
      y < room.top + room.height - RoomProps::free_border;
      y += 3
    ) {
      for(
        int x = room.left + RoomProps::free_border;
        x < room.left + room.width - RoomProps::free_border;
        ++x
      ) {
        if((x - room.left) % 7 == 0 || isInClearing(room, x, y)) continue;
        tryReserveScenery(
          room,
          room_index,
          x,
          y,
          SceneryKindEnum::server_rack,
          color
        );
      }
    }
  }

  void WorldFurnisher::furnishStorage(
    const RoomProps &room,
    std::size_t      room_index,
    ColorProps       color
  ) {
    RandomGenerator &random = world_.random();

    for(int attempt = 0; attempt < 5; ++attempt) {
      const int x = interiorX(room);
      const int y = interiorY(room);

      if(isInClearing(room, x, y)) continue;
      const std::size_t pile_size = random.index(4) + 2;

      for(std::size_t piece = 0; piece < pile_size; ++piece) {
        const SceneryKindEnum kind = random.chance(0.6F)
          ? SceneryKindEnum::crate
          : SceneryKindEnum::barrel;

        const int offset_x = random.intInclusive(-1, 1);
        const int offset_y = random.intInclusive(-1, 1);

        tryReserveScenery(
          room,
          room_index,
          x + offset_x,
          y + offset_y,
          kind,
          color
        );
      }
    }
  }

  void WorldFurnisher::furnishOffices(
    const RoomProps &room,
    std::size_t      room_index,
    ColorProps       color
  ) {
    for(int attempt = 0; attempt < 6; ++attempt) {
      const int x = interiorX(room);
      const int y = interiorY(room);

      if(!isInClearing(room, x, y))
        tryReserveScenery(
          room,
          room_index,
          x,
          y,
          SceneryKindEnum::terminal,
          color
        );
    }
  }

  void WorldFurnisher::furnishPumpStation(
    const RoomProps &room,
    std::size_t      room_index,
    ColorProps       color
  ) {
    RandomGenerator &random = world_.random();

    for(int attempt = 0; attempt < 7; ++attempt) {
      const int x = interiorX(room);
      const int y = interiorY(room);

      if(isInClearing(room, x, y)) continue;
      SceneryKindEnum kind = SceneryKindEnum::junction_box;

      if(random.chance(0.5F)) kind = SceneryKindEnum::pipes;
      else if(random.chance(0.5F)) kind = SceneryKindEnum::barrel;
      tryReserveScenery(room, room_index, x, y, kind, color);
    }
  }

  void WorldFurnisher::furnishHall(
    const RoomProps &room,
    std::size_t      room_index,
    ColorProps       color
  ) {
    RandomGenerator &random = world_.random();

    for(int attempt = 0; attempt < 3; ++attempt) {
      const int x = interiorX(room);
      const int y = interiorY(room);
      if(isInClearing(room, x, y)) continue;

      const SceneryKindEnum kind = random.chance(0.5F)
        ? SceneryKindEnum::junction_box
        : SceneryKindEnum::barrel;

      tryReserveScenery(room, room_index, x, y, kind, color);
    }
  }

  void WorldFurnisher::placeLockers(
    const RoomProps &room,
    std::size_t      room_index
  ) {
    if(room_index == world_.exitRoomIndex()) return;
    RandomGenerator &random = world_.random();
    TileMapModel    &map    = world_.map();

    const std::size_t locker_count
      = room_index == world_.startRoomIndex() ? 1 : random.index(2) + 1;
    std::size_t placed = 0;

    for(int attempt = 0; placed < locker_count && attempt < 30; ++attempt) {
      const bool faces_down = random.chance(0.5F);
      const int  x
        = random.intInclusive(room.left + 2, room.left + room.width - 3);

      const int y = faces_down ? room.top : room.top + room.height - 1;
      if(world_.isNearLandmark(x, y) || map.isWall(x, y)) continue;
      if(!map.reserveProp(x, y, 1, 1)) continue;
      if(!TileMapConnectivityAnalyzer::isRoomOpenAreaConnected(map, room)) {
        map.releaseProp(x, y, 1, 1);
        continue;
      }

      world_.addLocker(
        LockerEntity(TileMapModel::tileCenter(x, y), room_index, faces_down)
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
      SceneryKindEnum::neon_sign_bars,
    };

    RandomGenerator  &random     = world_.random();
    const std::size_t sign_count = random.index(2) + 1;

    for(std::size_t sign = 0; sign < sign_count; ++sign) {
      const int x
        = random.intInclusive(room.left + 3, room.left + room.width - 4);
      const TileMapModel &map             = world_.map();
      const bool          is_on_room_face = map.roomIndexAtTile(x, room.top)
        == room_index
        && !map.isWall(x, room.top)
        && map.isWall(x, room.top - 1);

      if(!is_on_room_face || world_.isNearLandmark(x, room.top)) continue;
      SceneryPlacementDto placement;
      placement.position
        = TileMapModel::tileCenter(x, room.top).sub(Vec2Props::of(0, 12));

      const SceneryKindEnum kind = kinds[random.index(kinds.size())];
      placement.mirrored         = random.chance(0.5F);
      placement.room_index       = room_index;
      placement.color
        = Palette::room_neon_colors[random
                                      .index(Palette::room_neon_colors.size())];
      placement.flicker_seed = random.unit();
      world_.addScenery(SceneryFactory::create(kind, placement));
    }
  }
} // namespace wheel_of_fear
