#include "mods/world/Navigation.hpp"

namespace wheel_of_fear {
  void Navigation::invalidate() {
    toward_player_.invalidate();
    for(DistanceField &field : cached_fields_) field.invalidate();
  }

  void Navigation::refreshTowardPlayer(
    const TileMap &map,
    Vec2           player_position
  ) {
    toward_player_.compute(map, player_position);
  }

  Vec2 Navigation::directionToPlayer(
    const TileMap &map,
    Vec2           from
  ) const {
    return toward_player_.directionAt(map, from);
  }

  Vec2 Navigation::directionToward(
    const TileMap &map,
    Vec2           from,
    Vec2           target,
    float          elapsed_seconds
  ) {
    if(from.distance(target) < direct_path_distance)
      return target.sub(from).normalized();
    clock_seconds_       = elapsed_seconds;
    DistanceField* field = fieldFor(map, target);
    if(field == nullptr) return target.sub(from).normalized();
    const Vec2 direction = field->directionAt(map, from);
    if(direction.length() < 0.5F) return target.sub(from).normalized();
    return direction;
  }

  DistanceField* Navigation::fieldFor(
    const TileMap &map,
    Vec2           target
  ) {
    const TileCoordinateProps target_tile = TileMap::tileAt(target);
    DistanceField*            oldest      = &cached_fields_[0];
    for(DistanceField &field : cached_fields_) {
      if(field.isFreshFor(map, target_tile)) {
        field.markUsed(clock_seconds_);
        return &field;
      }
      if(field.lastUsedSeconds() < oldest->lastUsedSeconds()) oldest = &field;
    }
    oldest->compute(map, target);
    oldest->markUsed(clock_seconds_);
    return oldest;
  }
} // namespace wheel_of_fear
