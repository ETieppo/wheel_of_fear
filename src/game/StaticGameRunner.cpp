#include "game/StaticGameRunner.hpp"
#include "core/storage/UserDataLocator.hpp"

namespace wheel_of_fear {
  std::unique_ptr<GameRunner> GameRunner::create(Window &window) {
    return std::make_unique<StaticGameRunner>(window);
  }

  StaticGameRunner::StaticGameRunner(Window &window)
      : game_(
          window,
          UserDataFilesProps {
            .progress = UserDataLocator::filePath(progress_file_name),
            .settings = UserDataLocator::filePath(settings_file_name),
          }
        ) {
  }

  bool StaticGameRunner::runFrame() {
    return game_.runFrame();
  }
} // namespace wheel_of_fear
