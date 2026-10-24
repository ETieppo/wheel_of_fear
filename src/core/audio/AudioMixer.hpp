#pragma once

#include "core/audio/AudioVoiceProps.hpp"
#include <SDL3/SDL_audio.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

namespace wheel_of_fear {
  using VoiceId                     = std::uint64_t;
  inline constexpr VoiceId no_voice = 0;

  class AudioMixer {
    public:
      static constexpr std::uint32_t channel_count = 2;
      static constexpr std::uint32_t sample_rate   = 48000;

      AudioMixer();
      ~AudioMixer();
      AudioMixer(const AudioMixer &)             = delete;
      AudioMixer &operator =(const AudioMixer &) = delete;

      bool isAvailable() const {
        return stream_ != nullptr;
      }

      VoiceId play(
        std::shared_ptr<const std::vector<float>> samples,
        float                                     volume,
        bool                                      is_looping
      );

      void setVolume(
        VoiceId voice,
        float   volume
      );

      void        stop(VoiceId voice);
      bool        isPlaying(VoiceId voice) const;
      std::size_t activeVoiceCount() const;
      void        setMasterVolume(float volume);
      void        releaseFinished();

    private:
      static void SDLCALL feed(
        void*            userdata,
        SDL_AudioStream* stream,
        int              additional_amount,
        int              total_amount
      );

      void                   mix(std::size_t sample_count);
      AudioVoiceProps*       find(VoiceId voice);
      const AudioVoiceProps* find(VoiceId voice) const;

      SDL_AudioStream*             stream_ = nullptr;
      mutable std::mutex           mutex_;
      std::vector<AudioVoiceProps> voices_;
      std::vector<float>           mix_buffer_;
      VoiceId                      next_id_       = 1;
      float                        master_volume_ = 1;
  };
} // namespace wheel_of_fear
