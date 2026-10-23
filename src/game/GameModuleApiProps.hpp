#pragma once

#include <cstddef>
#include <span>
#include <vector>

namespace wheel_of_fear {
  class Game;
  class Window;

  struct GameModuleApiProps {
      Game* (*create)(Window &window);
      void (*destroy)(Game* game);
      bool (*runFrame)(Game &game);
      std::vector<std::byte> (*saveSnapshot)(const Game &game);
      void (*restoreSnapshot)(
        Game                      &game,
        std::span<const std::byte> snapshot
      );
  };

  using GameModuleApiEntryPoint = const GameModuleApiProps* (*)();

  inline constexpr const char* game_module_api_symbol =
    "wheel_of_fear_game_module_api";
} // namespace wheel_of_fear
