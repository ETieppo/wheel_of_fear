#pragma once

#include "game/Game.hpp"
#include "game/GameRunner.hpp"

namespace wheel_of_fear {
  class StaticGameRunner final: public GameRunner {
    public:
      explicit StaticGameRunner(
        Window &window
      );

      bool runFrame() override;

    private:
      Game game_;
  };
} // namespace wheel_of_fear
