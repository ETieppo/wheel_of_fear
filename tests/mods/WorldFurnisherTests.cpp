#include "mods/world/WorldSystem.hpp"
#include <cstddef>
#include <cstdint>
#include <doctest/doctest.h>
#include <memory>

namespace wheel_of_fear {
  namespace {
    void checkLockerPlacements(
      std::uint32_t phase,
      std::uint64_t seed
    ) {
      const auto world = std::make_unique<WorldSystem>();
      world->generatePhase(phase, seed);
      const TileMapModel &map = world->map();

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

  TEST_CASE("lockers are mounted on a real wall of their room") {
    for(std::uint32_t phase = 1; phase <= 6; ++phase)
      for(std::uint64_t seed = 0; seed < 4; ++seed)
        checkLockerPlacements(phase, 0x7777 + (seed * 15485863));
  }
} // namespace wheel_of_fear
