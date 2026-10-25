#include "core/platform/ModuleExport.hpp"
#include "game/GameApplication.hpp"
#include "game/GameModuleApiProps.hpp"
#include <SDL3/SDL_log.h>
#include <cstring>
#include <optional>

namespace wheel_of_fear {
  namespace {
    GameApplication* createGame(Window &window) {
      return new GameApplication(window, UserDataFilesProps {});
    }

    void destroyGame(GameApplication* game) {
      delete game;
    }

    bool runGameFrame(GameApplication &game) {
      return game.runFrame();
    }

    std::vector<std::byte> saveGameSnapshot(const GameApplication &game) {
      const GameSnapshotProps snapshot = game.snapshot();
      std::vector<std::byte>  bytes(sizeof(GameSnapshotProps));
      std::memcpy(bytes.data(), &snapshot, sizeof(GameSnapshotProps));
      return bytes;
    }

    void restoreGameSnapshot(
      GameApplication           &game,
      std::span<const std::byte> bytes
    ) {
      if(bytes.size() != sizeof(GameSnapshotProps)) {
        SDL_Log("info(hot_reload): snapshot layout changed, starting fresh");
        return;
      }

      GameSnapshotProps snapshot;
      std::memcpy(&snapshot, bytes.data(), sizeof(GameSnapshotProps));
      game.resume(snapshot);
    }

    constexpr GameModuleApiProps game_module_api {
      .create          = createGame,
      .destroy         = destroyGame,
      .runFrame        = runGameFrame,
      .saveSnapshot    = saveGameSnapshot,
      .restoreSnapshot = restoreGameSnapshot,
    };
  } // namespace
} // namespace wheel_of_fear

WHEEL_OF_FEAR_MODULE_EXPORT const wheel_of_fear::GameModuleApiProps*
                                  wheel_of_fear_game_module_api() {
                                    return &wheel_of_fear::game_module_api;
                                  }
                                  