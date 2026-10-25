#include "core/items/ItemCatalog.hpp"
#include "core/theme/Palette.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/mobs/MobEntity.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/throwables/NeonPuddleHazard.hpp"
#include "mods/throwables/ThrowableSystem.hpp"
#include "mods/world/WorldSystem.hpp"
#include "support/HeadlessGameHarness.hpp"
#include <algorithm>
#include <cmath>
#include <doctest/doctest.h>
#include <memory>
#include <optional>
#include <set>
#include <utility>
#include <vector>

namespace wheel_of_fear {
  namespace {
    float glowIntensityAfter(float elapsed_seconds) {
      NeonPuddleHazard puddle(Vec2Props::of(0, 0), Palette::neon_magenta);
      LightingSystem   lighting;

      puddle.update(elapsed_seconds);
      puddle.emitLights(lighting, 0);

      return lighting.lights().front().intensity;
    }

    bool isOffTileCenter(Vec2Props point) {
      const TileCoordinateProps tile = TileMapModel::tileAt(point);
      const Vec2Props center         = TileMapModel::tileCenter(tile.x, tile.y);

      return center.distance(point) > 1;
    }

    const NeonSplashProps &waitForSplash(
      HeadlessGameHarness &harness,
      GameContextDto      &context
    ) {
      for(
        int frame = 0; frame < 600 && context.world.neonSplashes().empty();
        ++frame
      )
        harness.advanceFrames(1);

      REQUIRE_FALSE(context.world.neonSplashes().empty());

      return context.world.neonSplashes().front();
    }

    std::size_t connectedNeonTiles(
      const TileMapModel       &map,
      const TileCoordinateProps start
    ) {
      std::set<std::pair<int, int>> visited {
        {start.x, start.y}
      };
      std::vector<TileCoordinateProps> queue {start};

      for(std::size_t head = 0; head < queue.size(); ++head) {
        const TileCoordinateProps current = queue[head];

        for(
          const auto &[step_x, step_y] : {
            std::pair {1,  0 },
            std::pair {-1, 0 },
            std::pair {0,  1 },
            std::pair {0,  -1},
            std::pair {1,  1 },
            std::pair {1,  -1},
            std::pair {-1, 1 },
            std::pair {-1, -1}
        }
        ) {
          const int x = current.x + step_x;
          const int y = current.y + step_y;

          if(map.fluids().neonAtTile(x, y) <= 0) continue;
          if(!visited.insert({x, y}).second) continue;

          queue.push_back({.x = x, .y = y});
        }
      }

      return visited.size();
    }
  } // namespace

  TEST_CASE("the neon puddle lasts a minute and a half and dims gradually") {
    const float start   = glowIntensityAfter(1);
    const float halfway = glowIntensityAfter(45);
    const float late    = glowIntensityAfter(85);

    CHECK(halfway == doctest::Approx(start * 44.0F / 89.0F).epsilon(0.02));
    CHECK(late > 0);
    CHECK(late < halfway);

    NeonPuddleHazard puddle(Vec2Props::of(0, 0), Palette::neon_magenta);

    puddle.update(89);
    CHECK_FALSE(puddle.isDry());

    puddle.update(1);
    CHECK(puddle.isDry());
  }

  TEST_CASE(
    "the neon grenade lands as one paint splash with radiating spikes"
  ) {
    const auto      harness = std::make_unique<HeadlessGameHarness>("grenade");
    GameContextDto &context = harness->context();
    const NeonSplashProps &splash           = waitForSplash(*harness, context);
    const TileMapModel    &map              = context.world.map();
    int                    off_center_count = 0;

    REQUIRE(splash.droplets.size() > 20);

    for(const NeonDropletProps &droplet : splash.droplets) {
      CHECK(map.fluids().neonAt(droplet.position) > 0);

      if(isOffTileCenter(droplet.position)) off_center_count += 1;
    }

    const auto neon      = map.fluids().neonSeconds();
    const auto wet_tiles = static_cast<std::size_t>(
      std::count_if(neon.begin(), neon.end(), [](float seconds) {
        return seconds > 0;
      })
    );

    const Vec2Props center       = splash.droplets.front().position;
    const float     core_radius  = splash.droplets.front().radius;
    const auto      far_droplets = std::count_if(
      splash.droplets.begin(),
      splash.droplets.end(),
      [&](const NeonDropletProps &droplet) {
        return droplet.position.distance(center) > core_radius * 2.5F;
      }
    );

    CHECK(off_center_count > static_cast<int>(splash.droplets.size()) / 2);
    CHECK(wet_tiles > 20);
    CHECK(far_droplets >= 10);
    CHECK(
      connectedNeonTiles(
        map,
        TileMapModel::tileAt(splash.droplets.front().position)
      )
      == wet_tiles
    );
  }

  TEST_CASE("mobs walk onto the neon paint and burn") {
    const auto      harness = std::make_unique<HeadlessGameHarness>("grenade");
    GameContextDto &context = harness->context();
    const NeonSplashProps   &splash = waitForSplash(*harness, context);
    const Vec2Props          center = splash.droplets.front().position;
    const TileMapModel      &map    = context.world.map();
    std::optional<Vec2Props> outside;

    for(int step = 1; step < 12 && !outside; ++step) {
      const Vec2Props candidate
        = center.add(Vec2Props::of(0, 16.0F * static_cast<float>(step)));

      if(map.isSolidAt(candidate)) break;
      if(map.fluids().neonAt(candidate) <= 0) outside = candidate;
    }

    REQUIRE(outside.has_value());

    context.mobs.clear();
    context.mobs.spawn(MobKindEnum::crawler, *outside);
    harness->advanceFrames(2);
    REQUIRE(context.mobs.all().size() == 1);

    MobEntity      &mob     = *context.mobs.all().front();
    const Vec2Props inward  = center.sub(mob.position()).normalized();
    bool            entered = false;

    for(int frame = 0; frame < 120 && !entered; ++frame) {
      CHECK(mob.locomotion().moveAlong(context, inward, 40));
      entered = map.fluids().neonAt(mob.position()) > 0;
    }

    REQUIRE(entered);

    harness->advanceFrames(1);

    const bool is_burning_or_burnt = context.mobs.all().empty()
      || context.mobs.all().front()->condition().isBurning();
    CHECK(is_burning_or_burnt);
  }
} // namespace wheel_of_fear

namespace wheel_of_fear {
  TEST_CASE("neon grenade color changes with the phase and cycles") {
    const Item &grenade = ItemCatalog::of(ItemEnum::neon_grenade);
    const auto &colors  = Palette::grenade_neon_colors;

    CHECK(grenade.colorForPhase(1).r == colors[0].r);
    CHECK(grenade.colorForPhase(2).r == colors[1].r);
    CHECK(grenade.colorForPhase(3).g == colors[2].g);
    CHECK(
      grenade.colorForPhase(1 + colors.size()).b == grenade.colorForPhase(1).b
    );
  }
} // namespace wheel_of_fear
