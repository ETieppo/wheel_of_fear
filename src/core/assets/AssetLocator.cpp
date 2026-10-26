#include "core/assets/AssetLocator.hpp"
#include "core/assets/EmbeddedAssetCatalog.hpp"
#include <stdexcept>

namespace wheel_of_fear {
  std::filesystem::path
  AssetLocator::assetPath(std::string_view relative_path) {
    return std::filesystem::path("assets") / relative_path;
  }

  std::filesystem::path AssetLocator::shaderPath(std::string_view file_name) {
    return std::filesystem::path("shaders") / file_name;
  }

  std::vector<std::uint8_t>
  AssetLocator::readBytes(const std::filesystem::path &path) {
    const std::span<const std::uint8_t> bytes = embeddedBytes(path);

    return {bytes.begin(), bytes.end()};
  }

  std::string AssetLocator::readText(const std::filesystem::path &path) {
    const std::span<const std::uint8_t> bytes = embeddedBytes(path);

    return {bytes.begin(), bytes.end()};
  }

  std::span<const std::uint8_t>
  AssetLocator::embeddedBytes(const std::filesystem::path &path) {
    const std::string key   = path.generic_string();
    const auto        bytes = EmbeddedAssetCatalog::find(key);

    if(!bytes) throw std::runtime_error("unable to find embedded asset " + key);

    return *bytes;
  }
} // namespace wheel_of_fear
