#include "mods/mobs/species/MothMob.hpp"
#include "support/HeadlessGameHarness.hpp"
#include <algorithm>
#include <array>
#include <doctest/doctest.h>
#include <memory>
#include <optional>

namespace wheel_of_fear {
  namespace {
    constexpr Vec2Props perch_offset   = Vec2Props::of(-12, -14);
    constexpr Vec2Props flutter_extent = Vec2Props::of(7, 5);
    constexpr float     body_clearance = 6;

    bool isAreaSolid(
      const TileMapModel &map,
      Vec2Props           center,
      Vec2Props           extent
    ) {
      const std::array<Vec2Props, 5> probes {
        center,
        center.add(Vec2Props::of(-extent.x, -extent.y)),
        center.add(Vec2Props::of(extent.x, -extent.y)),
        center.add(Vec2Props::of(-extent.x, extent.y)),
        center.add(Vec2Props::of(extent.x, extent.y)),
      };

      return std::ranges::all_of(probes, [&map](Vec2Props probe) {
        return map.isSolidAt(probe);
      });
    }

    bool isAreaFree(
      const TileMapModel &map,
      Vec2Props           center,
      float               clearance
    ) {
      const std::array<Vec2Props, 5> probes {
        center,
        center.add(Vec2Props::of(-clearance, -clearance)),
        center.add(Vec2Props::of(clearance, -clearance)),
        center.add(Vec2Props::of(-clearance, clearance)),
        center.add(Vec2Props::of(clearance, clearance)),
      };

      return std::ranges::none_of(probes, [&map](Vec2Props probe) {
        return map.isSolidAt(probe);
      });
    }

    std::optional<Vec2Props> spotWithPerchInsideWall(const TileMapModel &map) {
      for(int y = 0; y < map.height(); ++y)
        for(int x = 0; x < map.width(); ++x) {
          const Vec2Props center = TileMapModel::tileCenter(x, y);

          for(int shift_step = -4; shift_step <= 4; ++shift_step) {
            const auto      shift = static_cast<float>(shift_step * 2);
            const Vec2Props spot  = center.add(Vec2Props::of(shift, shift));

            if(!map.roomIndexAt(spot)) continue;
            if(!isAreaFree(map, spot, body_clearance)) continue;
            if(isAreaSolid(map, spot.add(perch_offset), flutter_extent))
              return spot;
          }
        }

      return std::nullopt;
    }

    const MothMob* nearestMoth(
      GameContextDto &context,
      Vec2Props       point
    ) {
      const MothMob* nearest = nullptr;

      for(const auto &mob : context.mobs.all()) {
        if(mob->kind() != MobKindEnum::moth) continue;

        if(
          nearest
          != nullptr
          && mob->distanceTo(point)
          >= nearest->distanceTo(point)
        )
          continue;

        nearest = static_cast<const MothMob*>(mob.get());
      }

      return nearest;
    }
  } // namespace

  TEST_CASE(
    "the lantern moth floats through walls while it follows the player"
  ) {
    const auto      harness = std::make_unique<HeadlessGameHarness>("play");
    GameContextDto &context = harness->context();

    harness->advanceFrames(90);
    REQUIRE(context.clock.isRunning());

    const TileMapModel &map   = context.world.map();
    const auto          found = spotWithPerchInsideWall(map);
    REQUIRE(found.has_value());

    const Vec2Props spot   = found.value_or(Vec2Props {});
    const Vec2Props window = context.input.windowSize();
    context.input.simulateMousePosition(window.scale(0.95F));
    context.player.setPosition(spot);
    context.mobs.spawn(MobKindEnum::moth, spot);

    harness->advanceFrames(2);
    const MothMob* moth = nearestMoth(context, spot);
    REQUIRE(moth != nullptr);
    REQUIRE(moth->stance() == MothStanceEnum::following);

    for(int frame = 0; frame < 180; ++frame) {
      context.player.setPosition(spot);
      harness->advanceFrames(1);
    }

    moth = nearestMoth(context, spot);

    REQUIRE(moth != nullptr);
    CHECK(map.isSolidAt(moth->position()));
    CHECK(moth->position().distance(spot.add(perch_offset)) < 12);
    CHECK(
      context.lighting.freezingLightAt(map, spot, LightFilterEnum::any_light)
      > 0.5F
    );
  }
} // namespace wheel_of_fear
