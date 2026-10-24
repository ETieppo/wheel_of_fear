#pragma once

#include "core/math/Vec2Props.hpp"

namespace wheel_of_fear {
  class PlayerAimComponent {
    public:
      [[nodiscard]]
      Vec2Props direction() const {
        return direction_;
      }

      void reset() {
        direction_ = Vec2Props::of(1, 0);
      }

      void followMouse(
        Vec2Props holder_position,
        Vec2Props mouse_world_position
      );
      void pointAt(
        Vec2Props direction
      );

    private:
      static constexpr float minimum_mouse_distance = 2;

      Vec2Props direction_ = Vec2Props::of(1, 0);
  };
} // namespace wheel_of_fear
