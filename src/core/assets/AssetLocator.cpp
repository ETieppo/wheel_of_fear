#include "core/assets/AssetLocator.hpp"
#include <SDL3/SDL_filesystem.h>
#include <fstream>
#include <iterator>
#include <stdexcept>

namespace wheel_of_fear {
  std::filesystem::path AssetLocator::resolveDirectory(
    std::string_view bundled_name,
    std::string_view development_directory
  ) {
    if(const char* base_path = SDL_GetBasePath()) {
      const std::filesystem::path bundled
        = std::filesystem::path(base_path) / bundled_name;
      if(std::filesystem::is_directory(bundled)) return bundled;
    }

    return {development_directory};
  }

  std::filesystem::path
  AssetLocator::assetPath(std::string_view relative_path) {
    static const std::filesystem::path directory
      = resolveDirectory("assets", WHEEL_OF_FEAR_ASSET_DIRECTORY);
    return directory / relative_path;
  }

  std::filesystem::path AssetLocator::shaderPath(std::string_view file_name) {
    static const std::filesystem::path directory
      = resolveDirectory("shaders", WHEEL_OF_FEAR_SHADER_DIRECTORY);
    return directory / file_name;
  }

  std::vector<std::uint8_t>
  AssetLocator::readBytes(const std::filesystem::path &path) {
    std::ifstream stream(path, std::ios::binary);

    if(!stream)
      throw std::runtime_error("unable to open asset " + path.string());

    return {
      std::istreambuf_iterator<char>(stream),
      std::istreambuf_iterator<char>()
    };
  }

  std::string AssetLocator::readText(const std::filesystem::path &path) {
    std::ifstream stream(path, std::ios::binary);

    if(!stream)
      throw std::runtime_error("unable to open asset " + path.string());

    return {
      std::istreambuf_iterator<char>(stream),
      std::istreambuf_iterator<char>()
    };
  }
} // namespace wheel_of_fear
