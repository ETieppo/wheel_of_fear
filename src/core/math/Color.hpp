#pragma once

#include <array>
#include <cstdint>

namespace wheel_of_fear {
  struct Color {
      float r = 1;
      float g = 1;
      float b = 1;
      float a = 1;

      static constexpr
      Color white() {
        return Color {};
      }

      static constexpr
      Color black() {
        return Color {0, 0, 0, 1};
      }

      static constexpr
      Color hex(std::uint32_t value) {
        return Color {
          static_cast<float>((value >> 16) & 0xffU) / 255.0F,
          static_cast<float>((value >> 8) & 0xffU) / 255.0F,
          static_cast<float>(value & 0xffU) / 255.0F,
          1,
        };
      }

      constexpr
      Color withAlpha(float alpha) const {
        return Color {r, g, b, alpha};
      }

      constexpr
      Color mix(
        Color other,
        float amount
      ) const {
        return Color {
          r + (other.r - r) * amount,
          g + (other.g - g) * amount,
          b + (other.b - b) * amount,
          a + (other.a - a) * amount
        };
      }

      constexpr
      Color scaled(float factor) const {
        return Color {r * factor, g * factor, b * factor, a};
      }

      constexpr
      std::array<
        float,
        4
      > toArray() const {
        return {r, g, b, a};
      }
  };
} // namespace wheel_of_fear
