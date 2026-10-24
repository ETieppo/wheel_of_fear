#include "core/render/RenderConfig.hpp"
#include "core/render/Renderer.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/world/RoomPowerGrid.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <doctest/doctest.h>
#include <memory>

namespace wheel_of_fear {
  namespace {
    constexpr float       minimum_room_glow = 0.5F;
    constexpr std::size_t room_light_budget = 4;

    float colorDistance(
      ColorProps first,
      ColorProps second
    ) {
      return std::abs(first.r - second.r)
        + std::abs(first.g - second.g)
        + std::abs(first.b - second.b);
    }

    RoomPowerGrid fullyPoweredGrid(const WorldSystem &world) {
      RoomPowerGrid grid = world.power();

      for(std::size_t room = 0; room < world.map().rooms().size(); ++room)
        grid.power(room, true);

      grid.advance(10);
      return grid;
    }

    float glowAt(
      const Renderer &renderer,
      int             x,
      int             y
    ) {
      const std::size_t texel
        = ((static_cast<std::size_t>(y) * RenderConfig::max_map_size)
           + static_cast<std::size_t>(x))
        * 4;

      const auto &texels = renderer.frame().map_texels;
      return static_cast<float>(std::max(
               {texels[texel + 1], texels[texel + 2], texels[texel + 3]}
             ))
        / 255.0F;
    }

    template<typename CheckTile> void forEachFloorTile(
      const TileMapModel &map,
      std::size_t         room_index,
      CheckTile           check_tile
    ) {
      const RoomProps &room = map.rooms()[room_index];

      for(int y = room.top; y < room.top + room.height; ++y)
        for(int x = room.left; x < room.left + room.width; ++x) {
          if(map.isBlocked(x, y)) continue;
          if(map.roomIndexAtTile(x, y) != room_index) continue;

          check_tile(x, y);
        }
    }
  } // namespace

  TEST_CASE(
    "a fully powered room glows on every floor tile within its light budget"
  ) {
    for(std::uint32_t phase = 1; phase <= 6; ++phase)
      for(std::uint64_t seed = 0; seed < 3; ++seed) {
        const auto world = std::make_unique<WorldSystem>();
        world->generatePhase(phase, 0x7a11 + (seed * 7919));

        const TileMapModel &map      = world->map();
        const RoomPowerGrid grid     = fullyPoweredGrid(*world);
        const auto          renderer = std::make_unique<Renderer>();
        grid.paintRoomGlow(*renderer);

        for(std::size_t room = 0; room < map.rooms().size(); ++room) {
          LightingSystem lighting;
          float          dimmest = 1;
          grid.emitRoomLights(lighting, room);

          forEachFloorTile(map, room, [&](int x, int y) {
            dimmest = std::min(dimmest, glowAt(*renderer, x, y));
          });

          CAPTURE(phase);
          CAPTURE(seed);
          CAPTURE(room);
          CHECK(lighting.lights().size() <= room_light_budget);
          CHECK(dimmest >= minimum_room_glow);
        }
      }
  }

  TEST_CASE("an unpowered room stays dark") {
    const auto world = std::make_unique<WorldSystem>();
    world->generatePhase(2, 0x7a11);

    const TileMapModel &map      = world->map();
    const auto          renderer = std::make_unique<Renderer>();
    world->power().paintRoomGlow(*renderer);

    for(std::size_t room = 0; room < map.rooms().size(); ++room) {
      float brightest = 0;

      forEachFloorTile(map, room, [&](int x, int y) {
        brightest = std::max(brightest, glowAt(*renderer, x, y));
      });

      CAPTURE(room);
      CHECK(brightest == 0);
    }
  }

  TEST_CASE("every room strip blends two different colors along its length") {
    const auto world = std::make_unique<WorldSystem>();
    world->generatePhase(3, 0x7a11);
    const RoomPowerGrid grid = fullyPoweredGrid(*world);

    for(std::size_t room = 0; room < world->map().rooms().size(); ++room) {
      const float      perimeter    = grid.stripLength(room);
      const ColorProps start        = grid.ledColorAt(room, 0);
      const ColorProps wrapped      = grid.ledColorAt(room, perimeter - 0.01F);
      float            largest_step = 0;
      float            largest_contrast = 0;
      const auto       sample_count     = static_cast<int>(perimeter / 2);

      for(int sample = 0; sample < sample_count; ++sample) {
        const auto       along = static_cast<float>(sample * 2);
        const ColorProps here  = grid.ledColorAt(room, along);
        const float      step
          = colorDistance(here, grid.ledColorAt(room, along + 2));
        const float contrast
          = colorDistance(here, grid.ledColorAt(room, along + (perimeter / 2)));

        largest_step     = std::max(largest_step, step);
        largest_contrast = std::max(largest_contrast, contrast);
      }

      CAPTURE(room);
      CHECK(largest_contrast > 0.3F);
      CHECK(colorDistance(start, wrapped) < 0.01F);
      CHECK(largest_step < 0.05F);
    }
  }
} // namespace wheel_of_fear
