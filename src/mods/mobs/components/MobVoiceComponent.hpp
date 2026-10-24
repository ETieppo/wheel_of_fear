#pragma once

#include "core/audio/enums/SoundEnum.hpp"
#include "game/dtos/GameContextDto.hpp"

namespace wheel_of_fear {
  class MobEntity;

  class MobVoiceComponent {
    public:
      explicit MobVoiceComponent(MobEntity &owner)
          : owner_(owner) {
      }

      [[nodiscard]]
      float vocalCooldown() const {
        return vocal_cooldown_;
      }

      void setVocalCooldown(float seconds) {
        vocal_cooldown_ = seconds;
      }

      void tickCooldown(float delta_seconds);

      void playSound(
        GameContextDto &context,
        SoundEnum       sound,
        float           volume,
        float           pitch
      ) const;

      void vocalize(
        GameContextDto &context,
        float           chance
      );

      void playMovementSound(GameContextDto &context);

    private:
      static constexpr float movement_sound_distance = 300;
      static constexpr float occluded_sound_factor   = 0.45F;

      MobEntity &owner_;
      float      vocal_cooldown_          = 0;
      float      movement_sound_cooldown_ = 0;
  };
} // namespace wheel_of_fear
