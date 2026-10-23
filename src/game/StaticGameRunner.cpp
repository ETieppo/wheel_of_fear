#include "game/StaticGameRunner.hpp"

namespace wheel_of_fear {
  std::unique_ptr<GameRunner> GameRunner::create(Window &window) {
    return std::make_unique<StaticGameRunner>(window);
  }

  StaticGameRunner::StaticGameRunner(Window &window)
      : game_(window) {
  }

  bool StaticGameRunner::runFrame() {
    return game_.runFrame();
  }
} // namespace wheel_of_fear
