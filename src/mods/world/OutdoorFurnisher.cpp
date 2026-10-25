#include "mods/world/OutdoorFurnisher.hpp"
#include "core/render/RenderConfig.hpp"
#include "mods/world/WorldSystem.hpp"
#include "mods/world/scenery/SceneryFactory.hpp"
#include <cstdlib>

namespace wheel_of_fear {
  void OutdoorFurnisher::plantTrees(const RoomProps &clearing) {
    plantBorder(clearing);
    plantScattered(clearing);
  }

  void OutdoorFurnisher::plantBorder(const RoomProps &clearing) {
    RandomGenerator &random = world_.random();
    const int        right  = clearing.left + clearing.width - tree_footprint;
    const int        bottom = clearing.top + clearing.height - tree_footprint;

    for(int x = clearing.left; x <= right; x += border_spacing) {
      tryPlant(x, clearing.top + random.intInclusive(0, border_depth - 2));
      tryPlant(x, bottom - random.intInclusive(0, border_depth - 2));
    }

    for(int y = clearing.top; y <= bottom; y += border_spacing) {
      tryPlant(clearing.left + random.intInclusive(0, border_depth - 2), y);
      tryPlant(right - random.intInclusive(0, border_depth - 2), y);
    }
  }

  void OutdoorFurnisher::plantScattered(const RoomProps &clearing) {
    RandomGenerator &random = world_.random();

    for(int attempt = 0; attempt < scattered_attempts; ++attempt) {
      const int x = random.intInclusive(
        clearing.left + border_depth,
        clearing.left + clearing.width - border_depth - tree_footprint
      );
      const int y = random.intInclusive(
        clearing.top + border_depth,
        clearing.top + clearing.height - border_depth - tree_footprint
      );

      if(!isOnOpenPath(clearing, x, y)) tryPlant(x, y);
    }
  }

  bool OutdoorFurnisher::isOnOpenPath(
    const RoomProps &clearing,
    int              x,
    int              y
  ) const {
    const TileCoordinateProps start
      = TileMapModel::tileAt(world_.playerStart());
    const bool is_in_path_column
      = std::abs(x - clearing.centerTileX()) <= path_half_width;
    const bool is_near_start = std::abs(x - start.x)
      <= start_clear_radius
      && std::abs(y - start.y)
      <= start_clear_radius;

    return is_in_path_column || is_near_start;
  }

  bool OutdoorFurnisher::tryPlant(
    int x,
    int y
  ) {
    TileMapModel &map = world_.map();
    if(!map.reserveProp(x, y, tree_footprint, tree_footprint)) return false;

    RandomGenerator    &random = world_.random();
    SceneryPlacementDto placement;
    placement.position = TileMapModel::tileCenter(x, y).add(
      Vec2Props::of(RenderConfig::tile_size / 2, RenderConfig::tile_size / 2)
    );
    placement.mirrored = random.chance(0.5F);
    placement.variant  = random.index(tree_variant_count);

    world_.addScenery(SceneryFactory::create(SceneryKindEnum::tree, placement));
    return true;
  }
} // namespace wheel_of_fear
