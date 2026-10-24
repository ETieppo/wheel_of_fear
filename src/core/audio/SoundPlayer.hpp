#pragma once

#include "core/audio/AudioMixer.hpp"
#include "core/audio/MusicLayerTrack.hpp"
#include "core/audio/VoicePlaybackProps.hpp"
#include "core/audio/enums/MusicLayerEnum.hpp"
#include "core/audio/enums/SoundEnum.hpp"
#include "core/math/RandomGenerator.hpp"
#include "core/math/Vec2Props.hpp"
#include <array>
#include <memory>
#include <vector>

namespace wheel_of_fear {
  class SoundPlayer {
    public:
      void stop(VoiceId voice);
      bool isPlaying(VoiceId voice) const;
      void toggleMute();
      SoundPlayer();
      void updateMusic(float delta_seconds);
      void releaseFinished();

      void play(
        SoundEnum sound,
        float     volume,
        float     pan,
        float     pitch
      );

      VoiceId playTracked(
        SoundEnum sound,
        float     volume,
        float     pan,
        float     pitch
      );

      void playAt(
        SoundEnum sound,
        float     volume,
        Vec2Props listener,
        Vec2Props source,
        float     pitch
      );

      VoiceId playAtTracked(
        SoundEnum sound,
        float     volume,
        Vec2Props listener,
        Vec2Props source,
        float     pitch
      );

      void setMusicMix(
        float title,
        float ambience,
        float tension
      );

      static float spatialGain(
        Vec2Props listener,
        Vec2Props source
      );

    private:
      MusicLayerTrack &layer(MusicLayerEnum which);

      static constexpr std::size_t max_voices       = 48;
      static constexpr float       hearing_distance = 420;
      static constexpr float       stereo_width     = 160;

      static float spatialPan(
        Vec2Props listener,
        Vec2Props source
      );

      static std::vector<float> toMono(
        const std::vector<float> &samples,
        std::uint32_t             channels
      );

      static std::vector<float> toStereo(
        const std::vector<float> &samples,
        std::uint32_t             channels
      );

      VoiceId startVoice(
        SoundEnum                 sound,
        const VoicePlaybackProps &playback
      );

      AudioMixer                                               mixer_;
      std::array<std::vector<std::vector<float>>, sound_count> sound_bank_;
      std::vector<MusicLayerTrack>                             music_layers_;
      float           master_volume_ = 0.85F;
      float           music_volume_  = 0.7F;
      bool            is_muted_      = false;
      RandomGenerator random_ {0xA0D10};
  };
} // namespace wheel_of_fear
