#include "core/assets/EmbeddedAssetCatalog.hpp"
#include <algorithm>

namespace wheel_of_fear {
  std::optional<std::span<const std::uint8_t>>
  EmbeddedAssetCatalog::find(std::string_view path) {
    const std::span<const EmbeddedAssetProps> catalog = entries();

    const auto found
      = std::ranges::find(catalog, path, &EmbeddedAssetProps::path);

    if(found == catalog.end()) return std::nullopt;

    return found->bytes;
  }
} // namespace wheel_of_fear
