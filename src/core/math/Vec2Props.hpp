#pragma once

#include <cmath>

namespace wheel_of_fear {
  struct Vec2Props {
      float x = 0;
      float y = 0;

      static constexpr Vec2Props of(
        float x_value,
        float y_value
      ) {
        return Vec2Props {.x = x_value, .y = y_value};
      }

      static Vec2Props fromAngle(
        float radians
      ) {
        return Vec2Props {.x = std::cos(radians), .y = std::sin(radians)};
      }

      [[nodiscard]]
      constexpr Vec2Props add(
        Vec2Props other
      ) const {
        return Vec2Props {.x = x + other.x, .y = y + other.y};
      }

      [[nodiscard]]
      constexpr Vec2Props sub(
        Vec2Props other
      ) const {
        return Vec2Props {.x = x - other.x, .y = y - other.y};
      }

      [[nodiscard]]
      constexpr Vec2Props scale(
        float factor
      ) const {
        return Vec2Props {.x = x * factor, .y = y * factor};
      }

      [[nodiscard]]
      constexpr float dot(
        Vec2Props other
      ) const {
        return (x * other.x) + (y * other.y);
      }

      [[nodiscard]]
      float length() const {
        return std::sqrt((x * x) + (y * y));
      }

      [[nodiscard]]
      float distance(
        Vec2Props other
      ) const {
        return sub(other).length();
      }

      [[nodiscard]]
      Vec2Props normalized() const {
        const float magnitude = length();
        if(magnitude < 0.0001F) return Vec2Props {};

        return scale(1.0F / magnitude);
      }

      [[nodiscard]]
      constexpr Vec2Props lerp(
        Vec2Props other,
        float     t
      ) const {
        return Vec2Props {
          .x = x + ((other.x - x) * t),
          .y = y + ((other.y - y) * t),
        };
      }

      [[nodiscard]]
      float angle() const {
        return std::atan2(y, x);
      }
  };
} // namespace wheel_of_fear
