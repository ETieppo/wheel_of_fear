#include "mods/world/NavigationService.hpp"

namespace wheel_of_fear {
  void NavigationService::invalidate() {
    toward_player_.invalidate();
    for(DistanceField &field : cached_fields_) field.invalidate();
  }

  void NavigationService::refreshTowardPlayer(
    const TileMapModel &map,
    Vec2Props           player_position
  ) {
    toward_player_.compute(map, player_position);
  }

  Vec2Props NavigationService::directionToPlayer(
    const TileMapModel &map,
    Vec2Props           from
  ) const {
    return toward_player_.directionAt(map, from);
  }

  Vec2Props NavigationService::directionToward(
    const TileMapModel &map,
    Vec2Props           from,
    Vec2Props           target,
    float               elapsed_seconds
  ) {
    if(from.distance(target) < direct_path_distance)
      return target.sub(from).normalized();

    clock_seconds_ = elapsed_seconds;

    DistanceField* field = fieldFor(map, target);
    if(field == nullptr) return target.sub(from).normalized();

    const Vec2Props direction = field->directionAt(map, from);
    if(direction.length() < 0.5F) return target.sub(from).normalized();

    return direction;
  }

  DistanceField* NavigationService::fieldFor(
    const TileMapModel &map,
    Vec2Props           target
  ) {
    const TileCoordinateProps target_tile = TileMapModel::tileAt(target);
    DistanceField*            oldest      = cached_fields_.data();

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
