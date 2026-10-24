#pragma once

#include "mods/generators/states/GeneratorState.hpp"

namespace wheel_of_fear {
  class GeneratorOnlineState final: public GeneratorState {
    public:
      [[nodiscard]]
      float secondsOnline() const {
        return seconds_online_;
      }

      void enter() {
        seconds_online_ = 0;
      }

      [[nodiscard]]
      ColorProps statusColor(const GeneratorEntity &generator) const override;

      void update(
        GeneratorEntity          &generator,
        GameContextDto           &context,
        const GeneratorUpdateDto &update
      ) override;

    private:
      float seconds_online_ = 0;
  };
} // namespace wheel_of_fear
