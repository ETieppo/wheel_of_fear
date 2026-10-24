#pragma once

#include "game/GameApplication.hpp"
#include "game/GameRunner.hpp"

namespace wheel_of_fear {
  class StaticGameRunner final: public GameRunner {
    public:
      explicit StaticGameRunner(Window &window);
      bool runFrame() override;

    private:
      GameApplication game_;
  };
} // namespace wheel_of_fear
