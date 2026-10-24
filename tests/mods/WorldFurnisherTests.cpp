#include "mods/world/WorldSystem.hpp"
#include "mods/world/scenery/NeonSignScenery.hpp"
#include <cstddef>
#include <cstdint>
#include <doctest/doctest.h>
#include <memory>

namespace wheel_of_fear {
  namespace {
    void checkWallMountedPlacements(
      std::uint32_t phase,
      std::uint64_t seed
    ) {
      const auto world = std::make_unique<WorldSystem>();
      world->generatePhase(phase, seed);
      const TileMapModel &map = world->map();

      for(const auto &item : world->scenery()) {
        if(dynamic_cast<const NeonSignScenery*>(item.get()) == nullptr)
          continue;

        const TileCoordinateProps mount
          = TileMapModel::tileAt(item->position().add(Vec2Props::of(0, 12)));

        CAPTURE(phase);
        CAPTURE(seed);
        CHECK(map.roomIndexAtTile(mount.x, mount.y) == item->roomIndex());
        CHECK(map.isStructural(mount.x, mount.y - 1));
      }

      for(const LockerEntity &locker : world->lockers()) {
        const TileCoordinateProps tile
          = TileMapModel::tileAt(locker.position());

        CAPTURE(phase);
        CAPTURE(seed);
        CHECK(map.roomIndexAtTile(tile.x, tile.y) == locker.roomIndex());

        const bool backs_onto_wall = map.isStructural(tile.x, tile.y - 1)
          || map.isStructural(tile.x, tile.y + 1);
        CHECK(backs_onto_wall);
      }
    }
  } // namespace

  TEST_CASE("signs and lockers are mounted on a real wall of their room") {
    for(std::uint32_t phase = 1; phase <= 6; ++phase)
      for(std::uint64_t seed = 0; seed < 4; ++seed)
        checkWallMountedPlacements(phase, 0x7777 + (seed * 15485863));
  }
} // namespace wheel_of_fear
