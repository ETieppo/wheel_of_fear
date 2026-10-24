#include "game/HotReloadGameRunner.hpp"

#include <chrono>
#include <exception>
#include <stdexcept>
#include <vector>

#include <SDL3/SDL_log.h>

namespace wheel_of_fear {
  std::unique_ptr<GameRunner> GameRunner::create(
    Window &window
  ) {
    return std::make_unique<HotReloadGameRunner>(window);
  }

  HotReloadGameRunner::HotReloadGameRunner(
    Window &window
  )
      : window_(window)
      , watcher_(
          WHEEL_OF_FEAR_GAME_MODULE_PATH,
          WHEEL_OF_FEAR_GAME_MODULE_STAMP_PATH
        )
      , library_(watcher_.loadLatest())
      , api_(&resolveApi(*library_))
      , game_(api_->create(window_)) {
  }

  HotReloadGameRunner::~HotReloadGameRunner() {
    api_->destroy(game_);
  }

  bool HotReloadGameRunner::runFrame() {
    if(watcher_.hasUpdate()) reload();
    return api_->runFrame(*game_);
  }

  const GameModuleApiProps &HotReloadGameRunner::resolveApi(
    const SharedLibrary &library
  ) {
    const auto entry_point = reinterpret_cast<GameModuleApiEntryPoint>(
      library.function(game_module_api_symbol)
    );
    const GameModuleApiProps* api = entry_point();
    if(api == nullptr) throw std::runtime_error("game module returned no api");
    return *api;
  }

  Game* HotReloadGameRunner::startGame(
    const GameModuleApiProps  &api,
    std::span<const std::byte> snapshot
  ) const {
    Game* game = api.create(window_);
    try {
      api.restoreSnapshot(*game, snapshot);
    } catch(...) {
      api.destroy(game);
      throw;
    }
    return game;
  }

  void HotReloadGameRunner::reload() {
    const auto started_at = std::chrono::steady_clock::now();
    std::unique_ptr<SharedLibrary> next_library;
    const GameModuleApiProps*      next_api = nullptr;
    try {
      next_library = watcher_.loadLatest();
      next_api     = &resolveApi(*next_library);
    } catch(const std::exception &error) {
      SDL_Log("error(hot_reload): %s", error.what());
      return;
    }
    const std::vector<std::byte> snapshot = api_->saveSnapshot(*game_);
    api_->destroy(game_);
    game_ = nullptr;
    try {
      game_ = startGame(*next_api, snapshot);
    } catch(const std::exception &error) {
      SDL_Log("error(hot_reload): %s, keeping previous module", error.what());
      game_ = startGame(*api_, snapshot);
      return;
    }
    library_           = std::move(next_library);
    api_               = next_api;
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now() - started_at
    );
    SDL_Log(
      "info(hot_reload): game module reloaded in %lld ms",
      static_cast<long long>(elapsed.count())
    );
  }
} // namespace wheel_of_fear
