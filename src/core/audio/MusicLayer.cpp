#include "core/audio/MusicLayer.hpp"

#include "core/math/Scalar.hpp"

namespace wheel_of_fear {

void MusicLayer::update(AudioMixer& mixer, float delta_seconds, float mix_volume) {
    volume_ = Scalar::approach(volume_, target_volume_, delta_seconds * fade_per_second);
    if (volume_ <= 0.001F) {
        mixer.stop(voice_);
        voice_ = no_voice;
        return;
    }
    if (voice_ == no_voice || !mixer.isPlaying(voice_)) voice_ = mixer.play(samples_, 0, true);
    mixer.setVolume(voice_, volume_ * mix_volume);
}

}
