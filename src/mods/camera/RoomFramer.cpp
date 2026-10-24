#include "mods/camera/RoomFramer.hpp"

#include "core/math/ScalarMath.hpp"

namespace wheel_of_fear {
  Vec2Props RoomFramer::clampToRoom(
    const RoomProps &room,
    Vec2Props        desired
  ) {
    const Vec2Props minimum =
      room.worldMinimum().sub(Vec2Props::of(wall_margin, wall_margin));

    const Vec2Props maximum =
      room.worldMaximum().add(Vec2Props::of(wall_margin, wall_margin));

    return Vec2Props::of(
      clampAxis(desired.x, minimum.x, maximum.x, half_view.x),
      clampAxis(desired.y, minimum.y, maximum.y, half_view.y)
    );
  }

  float RoomFramer::clampAxis(
    float value,
    float minimum,
    float maximum,
    float half_extent
  ) {
    const float low  = minimum + half_extent;
    const float high = maximum - half_extent;
    if(low > high) return (minimum + maximum) / 2;

    return ScalarMath::clamp(value, low, high);
  }
} // namespace wheel_of_fear
