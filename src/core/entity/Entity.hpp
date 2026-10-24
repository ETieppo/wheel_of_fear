#pragma once

#include <cstddef>

#include "core/entity/Collider.hpp"
#include "core/math/Vec2Props.hpp"

namespace wheel_of_fear {
  class Entity {
    public:
      Entity() = default;
      Entity(
        Vec2Props position,
        float     collision_half_size
      );
      virtual ~Entity() = default;

      [[nodiscard]]
      Vec2Props position() const {
        return position_;
      }

      void setPosition(
        Vec2Props position
      ) {
        position_ = position;
      }

      [[nodiscard]]
      float heading() const {
        return heading_;
      }

      void setHeading(
        float heading
      ) {
        heading_ = heading;
      }

      [[nodiscard]]
      float collisionHalfSize() const {
        return collision_half_size_;
      }

      [[nodiscard]]
      float animationSeconds() const {
        return animation_seconds_;
      }

      [[nodiscard]]
      bool isAlive() const {
        return is_alive_;
      }

      void setAlive(
        bool is_alive
      ) {
        is_alive_ = is_alive;
      }

      [[nodiscard]]
      bool isMoving() const {
        return is_moving_;
      }

      void setMoving(
        bool is_moving
      ) {
        is_moving_ = is_moving;
      }

      void respawnAt(
        Vec2Props position
      );
      void advanceAnimation(
        float delta_seconds
      );
      [[nodiscard]]
      Vec2Props facing() const;
      [[nodiscard]]
      float distanceTo(
        Vec2Props point
      ) const;
      [[nodiscard]]
      Vec2Props directionTo(
        Vec2Props point
      ) const;
      void turnToward(
        float angle,
        float delta_seconds,
        float turn_rate
      );
      void faceToward(
        Vec2Props point
      );
      void slide(
        const Collider &collider,
        Vec2Props       displacement
      );
      [[nodiscard]]
      std::size_t animationFrame(
        float       frames_per_second,
        std::size_t frame_count
      ) const;
      [[nodiscard]]
      bool isFacing(
        Vec2Props point,
        float     minimum_cosine
      ) const;

    private:
      Vec2Props position_ {};
      float     heading_             = 0;
      float     collision_half_size_ = 4;
      float     animation_seconds_   = 0;
      bool      is_alive_            = true;
      bool      is_moving_           = false;
  };
} // namespace wheel_of_fear
