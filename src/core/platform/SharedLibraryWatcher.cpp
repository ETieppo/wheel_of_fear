#include "core/platform/SharedLibraryWatcher.hpp"
#include <chrono>
#include <string>
#include <system_error>
#include <utility>

namespace wheel_of_fear {
  SharedLibraryWatcher::SharedLibraryWatcher(
    std::filesystem::path library_path,
    std::filesystem::path stamp_path
  )
      : library_path_(std::move(library_path))
      , stamp_path_(std::move(stamp_path))
      , copies_directory_(
          std::filesystem::temp_directory_path() /
          (library_path_.stem().string() +
           "-" +
           std::to_string(
             std::chrono::system_clock::now().time_since_epoch().count()
           ))
        ) {
    std::filesystem::create_directories(copies_directory_);
  }

  SharedLibraryWatcher::~SharedLibraryWatcher() {
    std::error_code error;
    std::filesystem::remove_all(copies_directory_, error);
  }

  bool SharedLibraryWatcher::hasUpdate() const {
    const std::filesystem::file_time_type stamp_time = readStampTime();

    return stamp_time
      != std::filesystem::file_time_type::min()
      && stamp_time
      != loaded_stamp_time_;
  }

  std::unique_ptr<SharedLibrary> SharedLibraryWatcher::loadLatest() {
    loaded_stamp_time_                    = readStampTime();
    const std::filesystem::path copy_path = nextCopyPath();

    std::filesystem::copy_file(
      library_path_,
      copy_path,
      std::filesystem::copy_options::overwrite_existing
    );

    return std::make_unique<SharedLibrary>(copy_path);
  }

  std::filesystem::file_time_type SharedLibraryWatcher::readStampTime() const {
    std::error_code                       error;
    const std::filesystem::file_time_type stamp_time
      = std::filesystem::last_write_time(stamp_path_, error);

    return error ? std::filesystem::file_time_type::min() : stamp_time;
  }

  std::filesystem::path SharedLibraryWatcher::nextCopyPath() {
    generation_ += 1;

    return copies_directory_
      / (library_path_.stem().string()
         + "-"
         + std::to_string(generation_)
         + library_path_.extension().string());
  }
} // namespace wheel_of_fear
