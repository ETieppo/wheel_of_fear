#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>

#include "core/platform/SharedLibrary.hpp"

namespace wheel_of_fear {
  class SharedLibraryWatcher {
    public:
      SharedLibraryWatcher(
        std::filesystem::path library_path,
        std::filesystem::path stamp_path
      );
      ~SharedLibraryWatcher();
      SharedLibraryWatcher(
        const SharedLibraryWatcher &
      ) = delete;
      SharedLibraryWatcher &operator =(
        const SharedLibraryWatcher &
      ) = delete;

      bool                           hasUpdate() const;
      std::unique_ptr<SharedLibrary> loadLatest();

    private:
      std::filesystem::file_time_type readStampTime() const;
      std::filesystem::path           nextCopyPath();

      std::filesystem::path           library_path_;
      std::filesystem::path           stamp_path_;
      std::filesystem::path           copies_directory_;
      std::filesystem::file_time_type loaded_stamp_time_ {};
      std::uint32_t                   generation_ = 0;
  };
} // namespace wheel_of_fear
