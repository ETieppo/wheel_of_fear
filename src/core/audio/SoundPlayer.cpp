#include "core/audio/SoundPlayer.hpp"
#include "core/assets/AssetLocator.hpp"
#include "core/assets/SoundCatalog.hpp"
#include "core/audio/OggOpusDecoder.hpp"
#include "core/math/ScalarMath.hpp"
#include <SDL3/SDL_log.h>
#include <algorithm>
#include <cmath>
#include <exception>

namespace wheel_of_fear {
  SoundPlayer::SoundPlayer() {
    for(std::size_t sound_index = 0; sound_index < sound_count; ++sound_index) {
      for(
        const std::string_view file :
        SoundCatalog::filesFor(static_cast<SoundEnum>(sound_index))
      ) {
        try {
          const DecodedAudioProps decoded
            = OggOpusDecoder::decodeFile(AssetLocator::assetPath(file));
          sound_bank_[sound_index].push_back(
            toMono(decoded.samples, decoded.channels)
          );
        } catch(const std::exception &error) {
          SDL_Log(
            "sound %.*s unavailable: %s",
            static_cast<int>(file.size()),
            file.data(),
            error.what()
          );
        }
      }
    }

    for(
      std::size_t layer_index = 0; layer_index < music_layer_count;
      ++layer_index
    ) {
      std::vector<float> stereo;

      const std::string_view file
        = SoundCatalog::musicFor(static_cast<MusicLayerEnum>(layer_index));

      try {
        const DecodedAudioProps decoded
          = OggOpusDecoder::decodeFile(AssetLocator::assetPath(file));
        stereo = toStereo(decoded.samples, decoded.channels);
      } catch(const std::exception &error) {
        SDL_Log(
          "music %.*s unavailable: %s",
          static_cast<int>(file.size()),
          file.data(),
          error.what()
        );
      }

      music_layers_.emplace_back(
        std::make_shared<const std::vector<float>>(std::move(stereo))
      );
    }
  }

  std::vector<float> SoundPlayer::toMono(
    const std::vector<float> &samples,
    std::uint32_t             channels
  ) {
    const std::size_t  frame_count = samples.size() / channels;
    std::vector<float> mono(frame_count);

    for(std::size_t frame = 0; frame < frame_count; ++frame) {
      float sum = 0;

      for(std::uint32_t channel = 0; channel < channels; ++channel)
        sum += samples[(frame * channels) + channel];

      mono[frame] = sum / static_cast<float>(channels);
    }

    return mono;
  }

  std::vector<float> SoundPlayer::toStereo(
    const std::vector<float> &samples,
    std::uint32_t             channels
  ) {
    if(channels == AudioMixer::channel_count) return samples;
    const std::size_t  frame_count = samples.size() / channels;
    std::vector<float> stereo(frame_count * AudioMixer::channel_count);

    for(std::size_t frame = 0; frame < frame_count; ++frame) {
      const float value       = samples[frame * channels];
      stereo[frame * 2]       = value;
      stereo[(frame * 2) + 1] = value;
    }

    return stereo;
  }

  MusicLayerTrack &SoundPlayer::layer(MusicLayerEnum which) {
    return music_layers_[static_cast<std::size_t>(which)];
  }

  void SoundPlayer::releaseFinished() {
    mixer_.releaseFinished();
  }

  void SoundPlayer::toggleMute() {
    is_muted_ = !is_muted_;
    mixer_.setMasterVolume(is_muted_ ? 0.0F : 1.0F);
  }

  void SoundPlayer::setMusicMix(
    float title,
    float ambience,
    float tension
  ) {
    layer(MusicLayerEnum::title).setTargetVolume(title);
    layer(MusicLayerEnum::ambience).setTargetVolume(ambience);
    layer(MusicLayerEnum::tension).setTargetVolume(tension);
  }

  void SoundPlayer::updateMusic(float delta_seconds) {
    for(MusicLayerTrack &music_layer : music_layers_)
      music_layer.update(mixer_, delta_seconds, music_volume_ * master_volume_);
  }

  void SoundPlayer::play(
    SoundEnum sound,
    float     volume,
    float     pan,
    float     pitch
  ) {
    playTracked(sound, volume, pan, pitch);
  }

  VoiceId SoundPlayer::playTracked(
    SoundEnum sound,
    float     volume,
    float     pan,
    float     pitch
  ) {
    if(volume < 0.01F) return no_voice;

    return startVoice(
      sound,
      VoicePlaybackProps {.volume = volume, .pan = pan, .pitch = pitch}
    );
  }

  void SoundPlayer::playAt(
    SoundEnum sound,
    float     volume,
    Vec2Props listener,
    Vec2Props source,
    float     pitch
  ) {
    playAtTracked(sound, volume, listener, source, pitch);
  }

  VoiceId SoundPlayer::playAtTracked(
    SoundEnum sound,
    float     volume,
    Vec2Props listener,
    Vec2Props source,
    float     pitch
  ) {
    return playTracked(
      sound,
      volume * spatialGain(listener, source),
      spatialPan(listener, source),
      pitch
    );
  }

  float SoundPlayer::spatialGain(
    Vec2Props listener,
    Vec2Props source
  ) {
    const float falloff = ScalarMath::clamp(
      1.0F - (listener.distance(source) / hearing_distance),
      0,
      1
    );

    return falloff * falloff;
  }

  float SoundPlayer::spatialPan(
    Vec2Props listener,
    Vec2Props source
  ) {
    return ScalarMath::clamp((source.x - listener.x) / stereo_width, -1, 1);
  }

  void SoundPlayer::stop(VoiceId voice) {
    mixer_.stop(voice);
  }

  bool SoundPlayer::isPlaying(VoiceId voice) const {
    return mixer_.isPlaying(voice);
  }

  VoiceId SoundPlayer::startVoice(
    SoundEnum                 sound,
    const VoicePlaybackProps &playback
  ) {
    const auto &variants = sound_bank_[static_cast<std::size_t>(sound)];
    if(variants.empty() || mixer_.activeVoiceCount() >= max_voices)
      return no_voice;

    const std::vector<float> &source = variants[random_.index(variants.size())];
    if(source.empty()) return no_voice;

    const float step = playback.pitch;
    const auto  frame_count
      = static_cast<std::size_t>(static_cast<float>(source.size()) / step);
    const float left_gain
      = std::sqrt(ScalarMath::clamp(0.5F - (playback.pan * 0.5F), 0, 1))
      * ScalarMath::sqrt2;
    const float right_gain
      = std::sqrt(ScalarMath::clamp(0.5F + (playback.pan * 0.5F), 0, 1))
      * ScalarMath::sqrt2;

    auto samples = std::make_shared<std::vector<float>>(
      frame_count * AudioMixer::channel_count
    );

    for(std::size_t frame = 0; frame < frame_count; ++frame) {
      const float position = static_cast<float>(frame) * step;
      const auto  index    = static_cast<std::size_t>(position);
      const float fraction = position - static_cast<float>(index);
      const float current  = source[std::min(index, source.size() - 1)];
      const float next     = source[std::min(index + 1, source.size() - 1)];
      const float value    = current + ((next - current) * fraction);

      (*samples)[frame * 2]       = value * left_gain;
      (*samples)[(frame * 2) + 1] = value * right_gain;
    }

    return mixer_
      .play(std::move(samples), playback.volume * master_volume_, false);
  }
} // namespace wheel_of_fear
