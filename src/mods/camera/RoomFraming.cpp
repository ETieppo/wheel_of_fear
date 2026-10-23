#include "mods/camera/RoomFraming.hpp"

#include "core/math/Scalar.hpp"

namespace wheel_of_fear {

Vec2 RoomFraming::clampToRoom(const RoomProps& room, Vec2 desired) {
    const Vec2 minimum = room.worldMinimum().sub(Vec2::of(wall_margin, wall_margin));
    const Vec2 maximum = room.worldMaximum().add(Vec2::of(wall_margin, wall_margin));
    return Vec2::of(clampAxis(desired.x, minimum.x, maximum.x, half_view.x), clampAxis(desired.y, minimum.y, maximum.y, half_view.y));
}

float RoomFraming::clampAxis(float value, float minimum, float maximum, float half_extent) {
    const float low = minimum + half_extent;
    const float high = maximum - half_extent;
    if (low > high) return (minimum + maximum) / 2;
    return Scalar::clamp(value, low, high);
}

}
