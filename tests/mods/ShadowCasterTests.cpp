#include "core/render/RenderConfig.hpp"
#include "mods/world/WorldSystem.hpp"
#include "mods/world/scenery/Scenery.hpp"
#include "support/HeadlessGameHarness.hpp"
#include <cstdint>
#include <doctest/doctest.h>
#include <memory>

namespace wheel_of_fear {
  namespace {
    std::uint8_t wallTexelAt(
      const FrameProps         &frame,
      const TileCoordinateProps tile
    ) {
      const std::size_t texel_index
        = (static_cast<std::size_t>(tile.y) * RenderConfig::max_map_size)
        + static_cast<std::size_t>(tile.x);

      return frame.map_texels[texel_index * 4];
    }
  } // namespace

  TEST_CASE("server racks block light without becoming walls") {
    const auto harness = std::make_unique<HeadlessGameHarness>("play");
    harness->advanceFrames(2);

    const WorldSystem  &world               = harness->context().world;
    const TileMapModel &map                 = world.map();
    const FrameProps   &frame               = harness->frame();
    int                 shadow_caster_count = 0;

    for(const auto &item : world.scenery()) {
      if(!item->castsShadow()) continue;

      const TileCoordinateProps tile = TileMapModel::tileAt(item->position());

      CHECK_FALSE(map.isWall(tile.x, tile.y));
      CHECK(map.shadowCasters()[map.cellIndex(tile.x, tile.y)] == 1);
      CHECK(wallTexelAt(frame, tile) > 64);
      CHECK(wallTexelAt(frame, tile) < 128);

      shadow_caster_count += 1;
    }

    CHECK(shadow_caster_count > 0);
  }
} // namespace wheel_of_fear
