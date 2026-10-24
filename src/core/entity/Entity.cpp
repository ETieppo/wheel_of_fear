#include "core/entity/Entity.hpp"

#include <algorithm>
#include <cmath>

#include "core/math/Scalar.hpp"

namespace wheel_of_fear {
  Entity::Entity(
    Vec2  position,
    float collision_half_size
  )
      : position_(position)
      , collision_half_size_(collision_half_size) {
  }

  void Entity::respawnAt(
    Vec2 position
  ) {
    position_          = position;
    heading_           = 0;
    animation_seconds_ = 0;
    is_alive_          = true;
    is_moving_         = false;
  }

  void Entity::advanceAnimation(
    float delta_seconds
  ) {
    animation_seconds_ += delta_seconds;
  }

  Vec2 Entity::facing() const {
    return Vec2::fromAngle(heading_);
  }

  float Entity::distanceTo(
    Vec2 point
  ) const {
    return position_.distance(point);
  }

  Vec2 Entity::directionTo(
    Vec2 point
  ) const {
    return point.sub(position_).normalized();
  }

  void Entity::turnToward(
    float angle,
    float delta_seconds,
    float turn_rate
  ) {
    const float difference =
      std::atan2(std::sin(angle - heading_), std::cos(angle - heading_));
    heading_ += difference * Scalar::clamp(delta_seconds * turn_rate, 0, 1);
  }

  void Entity::faceToward(
    Vec2 point
  ) {
    const Vec2 offset = point.sub(position_);
    if(offset.length() > 0.01F) heading_ = offset.angle();
  }

  void Entity::slide(
    const Collider &collider,
    Vec2            displacement
  ) {
    const Vec2 before = position_;
    position_  = collider.slide(position_, displacement, collision_half_size_);
    is_moving_ = position_.distance(before) > 0.01F;
  }

  std::size_t Entity::animationFrame(
    float       frames_per_second,
    std::size_t frame_count
  ) const {
    if(frame_count == 0) return 0;
    return static_cast<std::size_t>(
             std::max(animation_seconds_ * frames_per_second, 0.0F)
           ) %
           frame_count;
  }

  bool Entity::isFacing(
    Vec2  point,
    float minimum_cosine
  ) const {
    const Vec2 toward = point.sub(position_).normalized();
    return toward.dot(facing()) >= minimum_cosine;
  }
} // namespace wheel_of_fear
