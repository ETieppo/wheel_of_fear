#pragma once

#include "game/dtos/GameContextDto.hpp"
#include "mods/tracker/TrackerGhostField.hpp"
#include "mods/tracker/TrackerSignalCollector.hpp"
#include <vector>

namespace wheel_of_fear {
  class TrackerSystem {
    public:
      void close();
      TrackerSystem();

      [[nodiscard]]
      bool isOpen() const {
        return is_open_;
      }

      [[nodiscard]]
      float openAmount() const {
        return open_amount_;
      }

      void forceOpen(bool is_open) {
        is_open_ = is_open;
      }

      void update(GameContextDto &context);

      void draw(GameContextDto &context) const;

    private:
      static constexpr float sweep_speed       = 2.4F;
      static constexpr float opening_speed     = 9;
      static constexpr float beep_noise_radius = 110;

      static bool crossedAngle(
        float previous,
        float current,
        float target
      );

      void toggleOnRequest(GameContextDto &context);

      void sweep(GameContextDto &context);

      bool                   is_open_     = false;
      float                  open_amount_ = 0;
      float                  sweep_angle_ = 0;
      TrackerSignalCollector signal_collector_;
      TrackerGhostField      ghost_field_;
      std::vector<float>     last_ping_seconds_;
  };
} // namespace wheel_of_fear
