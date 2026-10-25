#include "mods/director/ProgressStore.hpp"
#include "core/text/TextNumberParser.hpp"
#include <cstdint>
#include <format>
#include <utility>

namespace wheel_of_fear {
  ProgressStore::ProgressStore(std::optional<std::filesystem::path> file_path) {
    if(!file_path) return;

    storage_.emplace(std::move(*file_path));
    if(const std::optional<std::string> text = storage_->read())
      saved_ = parse(*text);
  }

  void ProgressStore::save(const DirectorSnapshotProps &snapshot) {
    if(!storage_ || !snapshot.is_in_phase) return;
    if(storage_->write(serialize(snapshot))) saved_ = snapshot;
  }

  void ProgressStore::clear() {
    saved_.reset();
    if(storage_) storage_->remove();
  }

  std::optional<DirectorSnapshotProps>
  ProgressStore::parse(std::string_view text) {
    std::optional<std::uint32_t> phase;
    std::optional<std::uint64_t> phase_seed;

    while(!text.empty()) {
      const std::size_t      line_end = text.find('\n');
      const std::string_view line     = text.substr(0, line_end);
      text = line_end == std::string_view::npos ? std::string_view {}
                                                : text.substr(line_end + 1);

      const std::size_t separator = line.find('=');
      if(separator == std::string_view::npos) continue;

      const std::string_view key   = line.substr(0, separator);
      const std::string_view value = line.substr(separator + 1);

      if(key == phase_key)
        phase = TextNumberParser::parse<std::uint32_t>(value);
      if(key == phase_seed_key)
        phase_seed = TextNumberParser::parse<std::uint64_t>(value);
    }

    if(!phase || *phase == 0 || !phase_seed) return std::nullopt;

    return DirectorSnapshotProps {
      .is_in_phase = true,
      .phase       = *phase,
      .phase_seed  = *phase_seed,
    };
  }

  std::string ProgressStore::serialize(const DirectorSnapshotProps &snapshot) {
    return std::format(
      "{}={}\n{}={}\n",
      phase_key,
      snapshot.phase,
      phase_seed_key,
      snapshot.phase_seed
    );
  }
} // namespace wheel_of_fear
