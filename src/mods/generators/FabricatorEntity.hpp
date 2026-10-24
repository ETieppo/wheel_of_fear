#pragma once

#include "core/audio/AudioMixer.hpp"
#include "core/entity/Entity.hpp"
#include "core/items/enums/ItemEnum.hpp"
#include "game/dtos/GameContextDto.hpp"
#include <cstddef>

namespace wheel_of_fear {
  class FabricatorEntity final: public Entity {
    public:
      FabricatorEntity(
        Vec2Props   position,
        std::size_t room_index,
        ItemEnum    produced_item
      );

      [[nodiscard]]
      std::size_t roomIndex() const {
        return room_index_;
      }

      [[nodiscard]]
      ItemEnum producedItem() const {
        return produced_item_;
      }

      [[nodiscard]]
      bool isReady() const {
        return cooldown_seconds_ <= 0;
      }

      [[nodiscard]]
      Vec2Props frontPoint() const;

      void update(GameContextDto &context);

      void draw(GameContextDto &context) const;

    private:
      void interrupt(GameContextDto &context);

      void work(GameContextDto &context);

      void deliver(GameContextDto &context);

      std::size_t room_index_;
      ItemEnum    produced_item_;
      float       work_progress_    = 0;
      float       cooldown_seconds_ = 0;
      float       noise_timer_      = 0;
      VoiceId     work_voice_       = no_voice;
  };
} // namespace wheel_of_fear
