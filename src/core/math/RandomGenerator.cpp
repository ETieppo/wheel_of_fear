#include "core/math/RandomGenerator.hpp"

#include "core/math/Scalar.hpp"

namespace wheel_of_fear {
  namespace {
    std::uint64_t splitMix(
      std::uint64_t &seed
    ) {
      seed += 0x9E3779B97F4A7C15ULL;
      std::uint64_t mixed = seed;
      mixed               = (mixed ^ (mixed >> 30)) * 0xBF58476D1CE4E5B9ULL;
      mixed               = (mixed ^ (mixed >> 27)) * 0x94D049BB133111EBULL;
      return mixed ^ (mixed >> 31);
    }

    std::uint64_t rotateLeft(
      std::uint64_t value,
      int           shift
    ) {
      return (value << shift) | (value >> (64 - shift));
    }
  } // namespace

  RandomGenerator::RandomGenerator(
    std::uint64_t seed
  ) {
    for(auto &word : state_) word = splitMix(seed);
  }

  std::uint64_t RandomGenerator::next() {
    const std::uint64_t result =
      rotateLeft(state_[0] + state_[3], 23) + state_[0];
    const std::uint64_t shifted = state_[1] << 17;
    state_[2] ^= state_[0];
    state_[3] ^= state_[1];
    state_[1] ^= state_[2];
    state_[0] ^= state_[3];
    state_[2] ^= shifted;
    state_[3] = rotateLeft(state_[3], 45);
    return result;
  }

  std::uint64_t RandomGenerator::below(
    std::uint64_t bound
  ) {
    if(bound == 0) return 0;
    const std::uint64_t threshold = (0 - bound) % bound;
    while(true) {
      const std::uint64_t value = next();
      if(value >= threshold) return value % bound;
    }
  }

  float RandomGenerator::unit() {
    return static_cast<float>(next() >> 40) * (1.0F / 16777216.0F);
  }

  float RandomGenerator::range(
    float low,
    float high
  ) {
    return low + (high - low) * unit();
  }

  int RandomGenerator::intInclusive(
    int low,
    int high
  ) {
    if(high <= low) return low;
    const auto span = static_cast<std::uint64_t>(
      static_cast<std::int64_t>(high) - static_cast<std::int64_t>(low) + 1
    );
    return low + static_cast<int>(below(span));
  }

  std::size_t RandomGenerator::index(
    std::size_t count
  ) {
    return static_cast<std::size_t>(below(count));
  }

  bool RandomGenerator::chance(
    float probability
  ) {
    return unit() < probability;
  }

  Vec2 RandomGenerator::direction() {
    return Vec2::fromAngle(range(0, Scalar::tau));
  }
} // namespace wheel_of_fear
