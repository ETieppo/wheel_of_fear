#pragma once

#include <cstddef>

#include "core/entity/Collider.hpp"
#include "core/math/Vec2.hpp"

namespace wheel_of_fear {
  class Entity {
    public:
      Entity() = default;
      Entity(
        Vec2  position,
        float collision_half_size
      );
      virtual ~Entity() = default;

      Vec2 position() const {
        return position_;
      }

      void setPosition(Vec2 position) {
        position_ = position;
      }

      float heading() const {
        return heading_;
      }

      void setHeading(float heading) {
        heading_ = heading;
      }

      float collisionHalfSize() const {
        return collision_half_size_;
      }

      float animationSeconds() const {
        return animation_seconds_;
      }

      bool isAlive() const {
        return is_alive_;
      }

      void setAlive(bool is_alive) {
        is_alive_ = is_alive;
      }

      bool isMoving() const {
        return is_moving_;
      }

      void setMoving(bool is_moving) {
        is_moving_ = is_moving;
      }

      void  respawnAt(Vec2 position);
      void  advanceAnimation(float delta_seconds);
      Vec2  facing() const;
      float distanceTo(Vec2 point) const;
      Vec2  directionTo(Vec2 point) const;
      void  turnToward(
        float angle,
        float delta_seconds,
        float turn_rate
      );
      void faceToward(Vec2 point);
      void slide(
        const Collider &collider,
        Vec2            displacement
      );
      std::size_t animationFrame(
        float       frames_per_second,
        std::size_t frame_count
      ) const;
      bool isFacing(
        Vec2  point,
        float minimum_cosine
      ) const;

    private:
      Vec2  position_ {};
      float heading_             = 0;
      float collision_half_size_ = 4;
      float animation_seconds_   = 0;
      bool  is_alive_            = true;
      bool  is_moving_           = false;
  };
} // namespace wheel_of_fear
