#pragma once

#include <array>
#include <cstdint>

namespace wheel_of_fear {
  struct ColorProps {
      float r = 1;
      float g = 1;
      float b = 1;
      float a = 1;

      static constexpr ColorProps white() {
        return ColorProps {};
      }

      static constexpr ColorProps black() {
        return ColorProps {.r = 0, .g = 0, .b = 0, .a = 1};
      }

      static constexpr ColorProps hex(
        std::uint32_t value
      ) {
        return ColorProps {
          .r = static_cast<float>((value >> 16) & 0xffU) / 255.0F,
          .g = static_cast<float>((value >> 8) & 0xffU) / 255.0F,
          .b = static_cast<float>(value & 0xffU) / 255.0F,
          .a = 1,
        };
      }

      [[nodiscard]]
      constexpr ColorProps withAlpha(
        float alpha
      ) const {
        return ColorProps {.r = r, .g = g, .b = b, .a = alpha};
      }

      [[nodiscard]]
      constexpr ColorProps mix(
        ColorProps other,
        float      amount
      ) const {
        return ColorProps {
          .r = r + ((other.r - r) * amount),
          .g = g + ((other.g - g) * amount),
          .b = b + ((other.b - b) * amount),
          .a = a + ((other.a - a) * amount),
        };
      }

      [[nodiscard]]
      constexpr ColorProps scaled(
        float factor
      ) const {
        return ColorProps {
          .r = r * factor,
          .g = g * factor,
          .b = b * factor,
          .a = a,
        };
      }

      [[nodiscard]]
      constexpr std::array<
        float,
        4
      > toArray() const {
        return {r, g, b, a};
      }
  };
} // namespace wheel_of_fear
