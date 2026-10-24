#pragma once

#include <algorithm>
#include <cstdint>
#include <numbers>

namespace wheel_of_fear {
  class ScalarMath final {
    public:
      static constexpr float pi    = std::numbers::pi_v<float>;
      static constexpr float tau   = 2.0F * pi;
      static constexpr float sqrt2 = std::numbers::sqrt2_v<float>;

      ScalarMath() = delete;

      static constexpr float clamp(
        float value,
        float low,
        float high
      ) {
        return std::max(low, std::min(high, value));
      }

      static constexpr float approach(
        float value,
        float target,
        float step
      ) {
        if(value < target) return std::min(value + step, target);
        return std::max(value - step, target);
      }

      static constexpr float smoothstep(
        float edge_low,
        float edge_high,
        float value
      ) {
        const float t
          = clamp((value - edge_low) / (edge_high - edge_low), 0, 1);
        return t * t * (3 - (2 * t));
      }

      static constexpr std::uint32_t hash(std::uint32_t value) {
        std::uint32_t mixed = value * 0x9E3779B1U;
        mixed ^= mixed >> 15;
        mixed *= 0x85EBCA77U;
        mixed ^= mixed >> 13;
        mixed *= 0xC2B2AE3DU;
        mixed ^= mixed >> 16;
        return mixed;
      }

      static constexpr float hashToUnit(std::uint32_t value) {
        return static_cast<float>(hash(value) & 0xFFFFFFU) / 16777216.0F;
      }

      static float hashTime(
        float seconds,
        float rate
      ) {
        return hashToUnit(
          static_cast<std::uint32_t>(std::max(seconds * rate, 0.0F))
        );
      }

      static float floorModulo(
        float value,
        float divisor
      );
  };
} // namespace wheel_of_fear
