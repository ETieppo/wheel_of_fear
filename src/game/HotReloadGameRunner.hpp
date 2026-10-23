#pragma once

#include <cstddef>
#include <memory>
#include <span>

#include "core/platform/SharedLibrary.hpp"
#include "core/platform/SharedLibraryWatcher.hpp"
#include "game/GameModuleApiProps.hpp"
#include "game/GameRunner.hpp"

namespace wheel_of_fear {
  class HotReloadGameRunner final: public GameRunner {
    public:
      explicit HotReloadGameRunner(Window &window);
      ~HotReloadGameRunner() override;
      HotReloadGameRunner(const HotReloadGameRunner &)             = delete;
      HotReloadGameRunner &operator =(const HotReloadGameRunner &) = delete;

      bool runFrame() override;

    private:
      static
      const GameModuleApiProps &resolveApi(const SharedLibrary &library);
      Game*                     startGame(
        const GameModuleApiProps  &api,
        std::span<const std::byte> snapshot
      ) const;
      void reload();

      Window                        &window_;
      SharedLibraryWatcher           watcher_;
      std::unique_ptr<SharedLibrary> library_;
      const GameModuleApiProps*      api_  = nullptr;
      Game*                          game_ = nullptr;
  };
} // namespace wheel_of_fear
