#pragma once

#include "core/storage/TextFileStorage.hpp"
#include "mods/director/DirectorSnapshotProps.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace wheel_of_fear {
  class ProgressStore {
    public:
      explicit ProgressStore(std::optional<std::filesystem::path> file_path);

      [[nodiscard]]
      const std::optional<DirectorSnapshotProps> &saved() const {
        return saved_;
      }

      void save(const DirectorSnapshotProps &snapshot);
      void clear();

    private:
      static constexpr std::string_view phase_key      = "phase";
      static constexpr std::string_view phase_seed_key = "phase_seed";

      static std::optional<DirectorSnapshotProps> parse(std::string_view text);
      static std::string serialize(const DirectorSnapshotProps &snapshot);

      std::optional<TextFileStorage>       storage_;
      std::optional<DirectorSnapshotProps> saved_;
  };
} // namespace wheel_of_fear
