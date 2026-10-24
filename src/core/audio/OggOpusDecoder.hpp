#pragma once

#include "core/audio/DecodedAudioProps.hpp"
#include <cstdint>
#include <filesystem>
#include <span>
#include <vector>

namespace wheel_of_fear {
  class OggOpusDecoder final {
    public:
      OggOpusDecoder() = delete;

      static DecodedAudioProps decodeFile(const std::filesystem::path &path);
      static DecodedAudioProps decode(std::span<const std::uint8_t> bytes);

    private:
      static std::vector<std::vector<std::uint8_t>> extractPackets(
        std::span<const std::uint8_t> bytes,
        std::int64_t                 &final_granule_position
      );
  };
} // namespace wheel_of_fear
