#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <set>
#include <vector>

#include <doctest/doctest.h>

#include "mods/generators/GeneratorSystem.hpp"
#include "mods/mobs/MobCatalog.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/world/TileMapConnectivityAnalyzer.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  namespace {
    std::size_t expectedGeneratorCount(
      std::uint32_t phase
    ) {
      return std::min<std::size_t>(2 + phase, 6);
    }

    std::size_t expectedFabricatorCount(
      std::uint32_t phase
    ) {
      return 3 + (phase / 2);
    }

    std::size_t expectedMimicCount(
      std::uint32_t phase
    ) {
      if(phase < MobCatalog::traitsOf(MobKindEnum::mimic).first_phase) return 0;
      return phase >= 7 ? 2 : 1;
    }

    std::size_t mimicCount(
      const MobSystem &mobs
    ) {
      return static_cast<std::size_t>(
        std::ranges::count_if(mobs.all(), [](const auto &mob) {
          return mob->kind() == MobKindEnum::mimic;
        })
      );
    }

    std::vector<bool> roomFootprint(
      const TileMapModel &map,
      std::size_t         room_index
    ) {
      const RoomProps  &room = map.rooms()[room_index];
      std::vector<bool> footprint;

      for(int y = room.top; y < room.top + room.height; ++y)
        for(int x = room.left; x < room.left + room.width; ++x)
          footprint.push_back(map.roomIndexAtTile(x, y) == room_index);

      return footprint;
    }
  } // namespace

  TEST_CASE(
    "rooms come in irregular, mutually different shapes"
  ) {
    const auto world = std::make_unique<WorldSystem>();

    world->generatePhase(4, 0x5151);

    const TileMapModel         &map = world->map();
    std::set<std::vector<bool>> distinct_shapes;
    std::size_t                 irregular_rooms = 0;

    for(
      std::size_t room_index = 0; room_index < map.rooms().size(); ++room_index
    ) {
      const std::vector<bool> footprint = roomFootprint(map, room_index);

      distinct_shapes.insert(footprint);
      if(std::ranges::count(footprint, false) > 0) irregular_rooms += 1;
    }

    CHECK(irregular_rooms * 2 > map.rooms().size());
    CHECK(distinct_shapes.size() * 2 > map.rooms().size());
  }

  TEST_CASE(
    "every phase places all of its generators, fabricators and mimics"
  ) {
    for(std::uint32_t phase = 1; phase <= 8; ++phase) {
      for(std::uint64_t seed = 0; seed < 6; ++seed) {
        CAPTURE(phase);
        CAPTURE(seed);

        const auto world      = std::make_unique<WorldSystem>();
        const auto generators = std::make_unique<GeneratorSystem>();
        const auto mobs       = std::make_unique<MobSystem>();

        world->generatePhase(phase, 0xABCD + (seed * 7919));
        generators->generatePhase(*world);
        mobs->populatePhase(*world);
        CHECK(generators->all().size() == expectedGeneratorCount(phase));

        CHECK(
          generators->fabricators().size() == expectedFabricatorCount(phase)
        );

        CHECK(mimicCount(*mobs) == expectedMimicCount(phase));
        CHECK(TileMapConnectivityAnalyzer::isRoomGraphConnected(world->map()));
        CHECK(TileMapConnectivityAnalyzer::isEveryFloorConnected(world->map()));
      }
    }
  }
} // namespace wheel_of_fear
