#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace wheel_of_fear {
  struct AudioVoiceProps {
      std::uint64_t                             id = 0;
      std::shared_ptr<const std::vector<float>> samples;
      std::size_t                               cursor     = 0;
      float                                     volume     = 1;
      bool                                      is_looping = false;
      bool                                      is_playing = true;
  };
} // namespace wheel_of_fear
