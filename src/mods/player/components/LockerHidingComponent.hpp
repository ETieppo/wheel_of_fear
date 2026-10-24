#pragma once

#include <cstddef>
#include <optional>

#include "core/entity/Entity.hpp"
#include "game/dtos/GameContextDto.hpp"

namespace wheel_of_fear {
  class LockerHidingComponent {
    public:
      [[nodiscard]]
      bool isHidden() const {
        return hidden_locker_index_.has_value();
      }

      [[nodiscard]]
      std::optional<std::size_t> hiddenLockerIndex() const {
        return hidden_locker_index_;
      }

      [[nodiscard]]
      std::optional<std::size_t> hideEventLockerIndex() const {
        return hide_event_locker_index_;
      }

      [[nodiscard]]
      float breath() const {
        return breath_;
      }

      [[nodiscard]]
      bool isHoldingBreath() const {
        return is_holding_breath_;
      }

      void reset();

      void clearHideEvent() {
        hide_event_locker_index_.reset();
      }

      [[nodiscard]]
      static std::optional<std::size_t> nearestLocker(
        const WorldSystem &world,
        Vec2Props          position
      );
      void enter(
        GameContextDto &context,
        Entity         &body,
        std::size_t     locker_index
      );
      void leave(
        GameContextDto &context,
        Entity         &body
      );
      void updateWhileHidden(
        GameContextDto &context,
        Entity         &body
      );

    private:
      static constexpr float breathing_noise_radius     = 34;
      static constexpr float gasp_noise_radius          = 170;
      static constexpr float locker_noise_radius        = 90;
      static constexpr float breath_hold_seconds        = 6;
      static constexpr float breathing_interval_seconds = 1.6F;

      std::optional<std::size_t> hidden_locker_index_;
      std::optional<std::size_t> hide_event_locker_index_;
      float                      breath_            = 1;
      bool                       is_holding_breath_ = false;
      float                      breathing_timer_   = 0;
  };
} // namespace wheel_of_fear
