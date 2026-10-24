#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "core/math/Vec2.hpp"

namespace wheel_of_fear {
  class RandomGenerator {
    public:
      explicit RandomGenerator(
        std::uint64_t seed
      );

      float unit();
      float range(
        float low,
        float high
      );
      int intInclusive(
        int low,
        int high
      );
      std::size_t index(
        std::size_t count
      );
      bool chance(
        float probability
      );
      Vec2 direction();

    private:
      std::uint64_t next();
      std::uint64_t below(
        std::uint64_t bound
      );

      std::array<std::uint64_t, 4> state_ {};
  };
} // namespace wheel_of_fear
