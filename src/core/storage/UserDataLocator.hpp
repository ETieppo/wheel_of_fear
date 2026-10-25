#pragma once

#include <filesystem>
#include <optional>
#include <string_view>

namespace wheel_of_fear {
  class UserDataLocator final {
    public:
      UserDataLocator() = delete;

      static std::optional<std::filesystem::path>
      filePath(std::string_view file_name);

    private:
      static constexpr const char* organization_name = "tieppo";
      static constexpr const char* application_name  = "wheel_of_fear";
  };
} // namespace wheel_of_fear
