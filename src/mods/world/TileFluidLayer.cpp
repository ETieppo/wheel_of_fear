#include "mods/world/TileFluidLayer.hpp"
#include "mods/world/TileMapModel.hpp"
#include <algorithm>

namespace wheel_of_fear {
  void TileFluidLayer::reset(std::size_t cell_count) {
    acid_seconds_.assign(cell_count, 0);
    neon_seconds_.assign(cell_count, 0);
  }

  void TileFluidLayer::clearCell(std::size_t cell) {
    acid_seconds_[cell] = 0;
    neon_seconds_[cell] = 0;
  }

  void TileFluidLayer::drainNeon(
    int x,
    int y
  ) {
    if(!map_.contains(x, y)) return;
    neon_seconds_[map_.cellIndex(x, y)] = 0;
  }

  float TileFluidLayer::acidAt(Vec2Props point) const {
    const TileCoordinateProps tile = TileMapModel::tileAt(point);
    if(!map_.contains(tile.x, tile.y)) return 0;
    return acid_seconds_[map_.cellIndex(tile.x, tile.y)];
  }

  void TileFluidLayer::spillAcid(
    Vec2Props point,
    float     seconds
  ) {
    const TileCoordinateProps tile = TileMapModel::tileAt(point);
    if(!map_.contains(tile.x, tile.y) || map_.isWall(tile.x, tile.y)) return;

    float &cell = acid_seconds_[map_.cellIndex(tile.x, tile.y)];
    cell        = std::max(cell, seconds);
  }

  float TileFluidLayer::neonAt(Vec2Props point) const {
    const TileCoordinateProps tile = TileMapModel::tileAt(point);
    return neonAtTile(tile.x, tile.y);
  }

  float TileFluidLayer::neonAtTile(
    int x,
    int y
  ) const {
    if(!map_.contains(x, y)) return 0;
    return neon_seconds_[map_.cellIndex(x, y)];
  }

  void TileFluidLayer::spillNeon(
    int   x,
    int   y,
    float seconds
  ) {
    if(!map_.contains(x, y) || map_.isWall(x, y)) return;
    float &cell = neon_seconds_[map_.cellIndex(x, y)];
    cell        = std::max(cell, seconds);
  }

  void TileFluidLayer::evaporate(float seconds) {
    for(float &acid : acid_seconds_)
      if(acid > 0) acid = std::max(0.0F, acid - seconds);

    for(float &neon : neon_seconds_)
      if(neon > 0) neon = std::max(0.0F, neon - seconds);
  }
} // namespace wheel_of_fear
