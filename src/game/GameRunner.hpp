#pragma once

#include <memory>

namespace wheel_of_fear {
  class Window;

  class GameRunner {
    public:
      virtual ~GameRunner() = default;

      virtual
      bool runFrame() = 0;

      static
      std::unique_ptr<GameRunner> create(Window &window);
  };
} // namespace wheel_of_fear
