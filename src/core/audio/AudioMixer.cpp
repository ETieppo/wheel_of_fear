#include "core/audio/AudioMixer.hpp"

#include <algorithm>

#include <SDL3/SDL_log.h>

namespace wheel_of_fear {

AudioMixer::AudioMixer() {
    const SDL_AudioSpec specification{SDL_AUDIO_F32, static_cast<int>(channel_count), static_cast<int>(sample_rate)};
    stream_ = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &specification, &AudioMixer::feed, this);
    if (stream_ == nullptr) {
        SDL_Log("audio unavailable: %s", SDL_GetError());
        return;
    }
    SDL_ResumeAudioStreamDevice(stream_);
}

AudioMixer::~AudioMixer() {
    if (stream_ != nullptr) SDL_DestroyAudioStream(stream_);
}

void SDLCALL AudioMixer::feed(void* userdata, SDL_AudioStream* stream, int additional_amount, int) {
    auto* mixer = static_cast<AudioMixer*>(userdata);
    if (additional_amount <= 0) return;
    const auto sample_count = static_cast<std::size_t>(additional_amount) / sizeof(float);
    mixer->mix(sample_count);
    SDL_PutAudioStreamData(stream, mixer->mix_buffer_.data(), static_cast<int>(sample_count * sizeof(float)));
}

void AudioMixer::mix(std::size_t sample_count) {
    const std::scoped_lock lock(mutex_);
    mix_buffer_.assign(sample_count, 0.0F);
    for (AudioVoiceProps& voice : voices_) {
        if (!voice.is_playing || !voice.samples || voice.samples->empty()) continue;
        const std::vector<float>& source = *voice.samples;
        const float gain = voice.volume * master_volume_;
        for (std::size_t index = 0; index < sample_count; ++index) {
            if (voice.cursor >= source.size()) {
                if (!voice.is_looping) {
                    voice.is_playing = false;
                    break;
                }
                voice.cursor = 0;
            }
            mix_buffer_[index] += source[voice.cursor] * gain;
            voice.cursor += 1;
        }
    }
    for (float& sample : mix_buffer_) sample = std::clamp(sample, -1.0F, 1.0F);
}

VoiceId AudioMixer::play(std::shared_ptr<const std::vector<float>> samples, float volume, bool is_looping) {
    if (stream_ == nullptr) return no_voice;
    const std::scoped_lock lock(mutex_);
    const VoiceId id = next_id_++;
    voices_.push_back(AudioVoiceProps{id, std::move(samples), 0, volume, is_looping, true});
    return id;
}

AudioVoiceProps* AudioMixer::find(VoiceId voice) {
    const auto found = std::find_if(voices_.begin(), voices_.end(), [voice](const AudioVoiceProps& candidate) { return candidate.id == voice; });
    return found == voices_.end() ? nullptr : &*found;
}

const AudioVoiceProps* AudioMixer::find(VoiceId voice) const {
    const auto found = std::find_if(voices_.begin(), voices_.end(), [voice](const AudioVoiceProps& candidate) { return candidate.id == voice; });
    return found == voices_.end() ? nullptr : &*found;
}

void AudioMixer::setVolume(VoiceId voice, float volume) {
    const std::scoped_lock lock(mutex_);
    if (AudioVoiceProps* found = find(voice)) found->volume = volume;
}

void AudioMixer::stop(VoiceId voice) {
    if (voice == no_voice) return;
    const std::scoped_lock lock(mutex_);
    if (AudioVoiceProps* found = find(voice)) found->is_playing = false;
}

bool AudioMixer::isPlaying(VoiceId voice) const {
    if (voice == no_voice) return false;
    const std::scoped_lock lock(mutex_);
    const AudioVoiceProps* found = find(voice);
    return found != nullptr && found->is_playing;
}

std::size_t AudioMixer::activeVoiceCount() const {
    const std::scoped_lock lock(mutex_);
    return voices_.size();
}

void AudioMixer::setMasterVolume(float volume) {
    const std::scoped_lock lock(mutex_);
    master_volume_ = volume;
}

void AudioMixer::releaseFinished() {
    const std::scoped_lock lock(mutex_);
    std::erase_if(voices_, [](const AudioVoiceProps& voice) { return !voice.is_playing; });
}

}
