#include "mods/mobs/MobBodyCollisionResolver.hpp"
#include "mods/mobs/MobCatalog.hpp"
#include "mods/mobs/MobEntity.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/mobs/species/CrawlerMob.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include "support/HeadlessGameHarness.hpp"
#include <doctest/doctest.h>
#include <memory>
#include <vector>

namespace wheel_of_fear {
  namespace {
    constexpr float crawler_spacing
      = CrawlerMob::species_traits.collision_radius
      * MobBodyCollisionResolver::body_radius_factor
      * 2;

    void resolveFor(
      std::vector<std::unique_ptr<MobEntity>> &mobs,
      PlayerEntity                            &player,
      const TileMapModel                      &map,
      int                                      frames
    ) {
      for(int frame = 0; frame < frames; ++frame)
        MobBodyCollisionResolver::resolve(mobs, player, map);
    }
  } // namespace

  TEST_CASE("crawlers push each other apart instead of overlapping") {
    const auto      harness = std::make_unique<HeadlessGameHarness>("play");
    GameContextDto &context = harness->context();

    harness->advanceFrames(2);

    const TileMapModel &map = context.world.map();
    const Vec2Props spot = map.rooms()[context.world.startRoomIndex()].center();
    std::vector<std::unique_ptr<MobEntity>> mobs;

    for(int index = 0; index < 4; ++index)
      mobs.push_back(MobCatalog::create(MobKindEnum::crawler, spot));

    context.player.setPosition(spot.add(Vec2Props::of(200, 0)));
    resolveFor(mobs, context.player, map, 30);

    for(std::size_t first = 0; first < mobs.size(); ++first) {
      CHECK_FALSE(map.isSolidAt(mobs[first]->position()));

      for(std::size_t second = first + 1; second < mobs.size(); ++second)
        CHECK(
          mobs[first]->position().distance(mobs[second]->position())
          >= crawler_spacing
          * 0.95F
        );
    }
  }

  TEST_CASE(
    "the player shoves a crawler, but a frozen crawler blocks the player"
  ) {
    const auto      harness = std::make_unique<HeadlessGameHarness>("play");
    GameContextDto &context = harness->context();

    harness->advanceFrames(2);

    const TileMapModel &map = context.world.map();
    const Vec2Props spot = map.rooms()[context.world.startRoomIndex()].center();
    const float     spacing
      = (crawler_spacing / 2) + context.player.collisionHalfSize();
    std::vector<std::unique_ptr<MobEntity>> mobs;

    mobs.push_back(MobCatalog::create(MobKindEnum::crawler, spot));
    context.player.setPosition(spot.add(Vec2Props::of(2, 0)));
    resolveFor(mobs, context.player, map, 10);

    CHECK(
      context.player.position().distance(spot.add(Vec2Props::of(2, 0))) < 0.01F
    );
    CHECK(
      mobs.front()->position().distance(context.player.position())
      >= spacing
      * 0.95F
    );

    const Vec2Props frozen_spot = mobs.front()->position();

    mobs.front()->condition().setFrozen(true);
    context.player.setPosition(frozen_spot.add(Vec2Props::of(2, 0)));
    resolveFor(mobs, context.player, map, 10);

    CHECK(mobs.front()->position().distance(frozen_spot) < 0.01F);
    CHECK(
      mobs.front()->position().distance(context.player.position())
      >= spacing
      * 0.95F
    );
  }

  TEST_CASE("a crawler body still lets it strike the player") {
    constexpr float player_spacing
      = (crawler_spacing / 2) + PlayerEntity::collision_half_size;

    CHECK(player_spacing < CrawlerMob::species_traits.strike_reach);
  }

  TEST_CASE(
    "crawlers spawned on the same spot spread out while the game runs"
  ) {
    const auto      harness = std::make_unique<HeadlessGameHarness>("play");
    GameContextDto &context = harness->context();

    harness->advanceFrames(90);
    REQUIRE(context.clock.isRunning());

    const Vec2Props spot = context.world.map()
                             .rooms()[context.world.startRoomIndex()]
                             .center()
                             .add(Vec2Props::of(0, -40));

    context.mobs.clear();
    context.mobs.setGraceSeconds(1000);

    for(int index = 0; index < 3; ++index)
      context.mobs.spawn(MobKindEnum::crawler, spot);

    harness->advanceFrames(60);

    const auto &mobs = context.mobs.all();
    REQUIRE(mobs.size() == 3);

    for(std::size_t first = 0; first < mobs.size(); ++first)
      for(std::size_t second = first + 1; second < mobs.size(); ++second)
        CHECK(
          mobs[first]->position().distance(mobs[second]->position())
          >= crawler_spacing
          * 0.9F
        );
  }
} // namespace wheel_of_fear
