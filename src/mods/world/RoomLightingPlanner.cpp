#include "mods/world/RoomLightingPlanner.hpp"
#include <algorithm>
#include <cmath>

namespace wheel_of_fear {
  int RoomLightingPlanner::cellCount(
    float extent,
    int   maximum
  ) {
    const auto wanted = static_cast<int>(std::ceil(extent / target_cell_size));
    return std::clamp(wanted, 1, maximum);
  }

  RoomLightingPlanProps RoomLightingPlanner::plan(
    const TileMapModel &map,
    std::size_t         room_index,
    const RoomLedStrip &strip
  ) {
    return RoomLightingPlanProps {
      .fill_lights = planFillLights(map, room_index, strip),
      .glow_tiles  = planGlowTiles(map, room_index, strip),
      .fill_bounds = planFillBounds(map.rooms()[room_index]),
    };
  }

  LightBoundsProps RoomLightingPlanner::planFillBounds(const RoomProps &room) {
    const Vec2Props margin = Vec2Props::of(
      RenderConfig::tile_size * wall_margin_tiles,
      RenderConfig::tile_size * wall_margin_tiles
    );

    return LightBoundsProps {
      .minimum = room.worldMinimum().sub(margin),
      .maximum = room.worldMaximum().add(margin),
    };
  }

  std::vector<RoomFillLightProps> RoomLightingPlanner::planFillLights(
    const TileMapModel &map,
    std::size_t         room_index,
    const RoomLedStrip &strip
  ) {
    const RoomProps &room    = map.rooms()[room_index];
    const Vec2Props  minimum = room.worldMinimum();
    const Vec2Props  size    = room.worldMaximum().sub(minimum);
    const int        columns = cellCount(size.x, maximum_columns);
    const int        rows    = cellCount(size.y, maximum_rows);
    const Vec2Props  cell    = Vec2Props::of(
      size.x / static_cast<float>(columns),
      size.y / static_cast<float>(rows)
    );
    const float radius = std::max(
      minimum_radius,
      cell.scale(0.5F).length() * reach_per_half_cell
    );

    std::vector<RoomFillLightProps> lights;

    for(int row = 0; row < rows; ++row)
      for(int column = 0; column < columns; ++column) {
        const Vec2Props cell_center = minimum.add(
          Vec2Props::of(
            (static_cast<float>(column) + 0.5F) * cell.x,
            (static_cast<float>(row) + 0.5F) * cell.y
          )
        );

        const Vec2Props position = map.isSolidAt(cell_center)
          ? map.nearestFloor(cell_center)
          : cell_center;

        lights.push_back(
          RoomFillLightProps {
            .position       = position,
            .radius         = radius,
            .distance_along = strip.nearestDistanceAlong(position),
          }
        );
      }

    return lights;
  }

  std::vector<RoomGlowTileProps> RoomLightingPlanner::planGlowTiles(
    const TileMapModel &map,
    std::size_t         room_index,
    const RoomLedStrip &strip
  ) {
    const RoomProps               &room = map.rooms()[room_index];
    std::vector<RoomGlowTileProps> tiles;

    for(int y = room.top; y < room.top + room.height; ++y)
      for(int x = room.left; x < room.left + room.width; ++x) {
        if(map.roomIndexAtTile(x, y) != room_index) continue;

        tiles.push_back(
          RoomGlowTileProps {
            .x = x,
            .y = y,
            .distance_along
            = strip.nearestDistanceAlong(TileMapModel::tileCenter(x, y)),
          }
        );
      }

    return tiles;
  }
} // namespace wheel_of_fear
