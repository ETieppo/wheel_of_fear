#pragma once

#include <memory>
#include <vector>

#include "core/audio/AudioMixer.hpp"

namespace wheel_of_fear {
  class MusicLayerTrack {
    public:
      explicit MusicLayerTrack(
        std::shared_ptr<const std::vector<float>> stereo_samples
      )
          : samples_(std::move(stereo_samples)) {
      }

      void setTargetVolume(
        float target_volume
      ) {
        target_volume_ = target_volume;
      }

      void update(
        AudioMixer &mixer,
        float       delta_seconds,
        float       mix_volume
      );

    private:
      static constexpr float fade_per_second = 0.6F;

      std::shared_ptr<const std::vector<float>> samples_;
      VoiceId                                   voice_         = no_voice;
      float                                     volume_        = 0;
      float                                     target_volume_ = 0;
  };
} // namespace wheel_of_fear
