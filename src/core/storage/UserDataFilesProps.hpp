#pragma once

#include <filesystem>
#include <optional>

namespace wheel_of_fear {
  struct UserDataFilesProps {
      std::optional<std::filesystem::path> progress;
      std::optional<std::filesystem::path> settings;
  };
} // namespace wheel_of_fear
