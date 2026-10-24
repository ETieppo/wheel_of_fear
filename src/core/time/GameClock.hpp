#pragma once

namespace wheel_of_fear {
  class GameClock {
    public:
      void advance(float real_delta_seconds);

      [[nodiscard]]
      bool isRunning() const {
        return is_running_;
      }

      void setRunning(bool is_running) {
        is_running_ = is_running;
      }

      [[nodiscard]]
      float deltaSeconds() const {
        return delta_seconds_;
      }

      [[nodiscard]]
      float realDeltaSeconds() const {
        return real_delta_seconds_;
      }

      [[nodiscard]]
      float realSeconds() const {
        return real_seconds_;
      }

    private:
      bool  is_running_         = false;
      float delta_seconds_      = 0;
      float real_delta_seconds_ = 0;
      float real_seconds_       = 0;
  };
} // namespace wheel_of_fear
