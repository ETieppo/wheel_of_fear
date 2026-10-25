#pragma once

#include "core/math/ScalarMath.hpp"
#include "core/math/Vec2Props.hpp"
#include <array>
#include <cmath>
#include <cstddef>

namespace wheel_of_fear {
  struct NeonSplashOutlineProps {
      static constexpr std::size_t sample_count = 48;

      Vec2Props                       center {};
      float                           base_radius = 0;
      std::array<float, sample_count> radii {};

      [[nodiscard]]
      float radiusAt(float angle) const {
        const float turns = angle / ScalarMath::tau;
        const float position
          = (turns - std::floor(turns)) * static_cast<float>(sample_count);
        const auto  first  = static_cast<std::size_t>(position) % sample_count;
        const auto  second = (first + 1) % sample_count;
        const float blend  = position - std::floor(position);

        return radii[first] + ((radii[second] - radii[first]) * blend);
      }

      [[nodiscard]]
      bool contains(
        Vec2Props point,
        float     margin
      ) const {
        const Vec2Props offset = point.sub(center);

        return offset.length() <= radiusAt(offset.angle()) + margin;
      }
  };
} // namespace wheel_of_fear
