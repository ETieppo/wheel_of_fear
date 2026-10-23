#pragma once

#include <cstdint>
#include <vector>

namespace wheel_of_fear {
  struct DecodedAudioProps {
      std::vector<float> samples;
      std::uint32_t      channels    = 1;
      std::uint32_t      sample_rate = 48000;
  };
} // namespace wheel_of_fear
