#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace wheel_of_fear {
  class TextFileStorage {
    public:
      explicit TextFileStorage(std::filesystem::path path);

      [[nodiscard]]
      std::optional<std::string> read() const;

      bool write(std::string_view text) const;
      void remove() const;

    private:
      std::filesystem::path path_;
  };
} // namespace wheel_of_fear
