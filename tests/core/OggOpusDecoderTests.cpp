#include <doctest/doctest.h>

#include "core/assets/AssetLocator.hpp"
#include "core/audio/OggOpusDecoder.hpp"

namespace wheel_of_fear {
  TEST_CASE("decodes the bundled ogg opus effects at 48 kHz") {
    const DecodedAudioProps footstep = OggOpusDecoder::decodeFile(
      AssetLocator::assetPath("audio/sfx/footstep_0.opus")
    );
    CHECK(footstep.channels == 1);
    CHECK(footstep.sample_rate == 48000);
    CHECK(footstep.samples.size() > 1000);
    const DecodedAudioProps title = OggOpusDecoder::decodeFile(
      AssetLocator::assetPath("audio/music/title.opus")
    );
    CHECK(title.channels == 2);
    CHECK(title.samples.size() % 2 == 0);
  }
} // namespace wheel_of_fear
