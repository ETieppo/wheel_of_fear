#pragma once

#include <cstddef>
#include <span>
#include <vector>

namespace wheel_of_fear {
  class GameApplication;
  class Window;

  struct GameModuleApiProps {
      GameApplication* (*create)(Window &window);
      void (*destroy)(GameApplication* game);
      bool (*runFrame)(GameApplication &game);
      std::vector<std::byte> (*saveSnapshot)(const GameApplication &game);
      void (*restoreSnapshot)(
        GameApplication                      &game,
        std::span<const std::byte> snapshot
      );
  };

  using GameModuleApiEntryPoint = const GameModuleApiProps* (*)();

  inline constexpr const char* game_module_api_symbol =
    "wheel_of_fear_game_module_api";
} // namespace wheel_of_fear
