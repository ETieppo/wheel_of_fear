#include "core/assets/AssetLocator.hpp"
#include "core/assets/EmbeddedAssetCatalog.hpp"
#include <cstddef>
#include <cstdint>
#include <doctest/doctest.h>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace wheel_of_fear {
  namespace {
    constexpr std::string_view asset_prefix  = "assets/";
    constexpr std::string_view shader_prefix = "shaders/";

    std::filesystem::path sourcePathOf(std::string_view embedded_path) {
      if(embedded_path.starts_with(asset_prefix))
        return std::filesystem::path(WHEEL_OF_FEAR_ASSET_DIRECTORY)
          / embedded_path.substr(asset_prefix.size());

      return std::filesystem::path(WHEEL_OF_FEAR_SHADER_DIRECTORY)
        / embedded_path.substr(shader_prefix.size());
    }

    std::vector<std::uint8_t>
    readSourceFile(const std::filesystem::path &path) {
      std::ifstream stream(path, std::ios::binary);

      return {
        std::istreambuf_iterator<char>(stream),
        std::istreambuf_iterator<char>()
      };
    }

    std::size_t countSourceFiles(const std::filesystem::path &directory) {
      std::size_t count = 0;

      for(
        const std::filesystem::directory_entry &entry :
        std::filesystem::recursive_directory_iterator(directory)
      )
        if(entry.is_regular_file()) ++count;

      return count;
    }
  } // namespace

  TEST_CASE("every embedded asset matches its source file byte for byte") {
    for(const EmbeddedAssetProps &asset : EmbeddedAssetCatalog::entries()) {
      CAPTURE(asset.path);

      const std::vector<std::uint8_t> source_bytes
        = readSourceFile(sourcePathOf(asset.path));

      CHECK(
        std::vector<std::uint8_t>(asset.bytes.begin(), asset.bytes.end())
        == source_bytes
      );
    }
  }

  TEST_CASE("the embedded catalog holds every asset and shader file") {
    const std::size_t source_file_count
      = countSourceFiles(WHEEL_OF_FEAR_ASSET_DIRECTORY)
      + countSourceFiles(WHEEL_OF_FEAR_SHADER_DIRECTORY);

    CHECK(EmbeddedAssetCatalog::entries().size() == source_file_count);
  }

  TEST_CASE("asset paths resolve to embedded bytes without the source tree") {
    CHECK_FALSE(
      AssetLocator::readBytes(AssetLocator::shaderPath("quad.wgsl")).empty()
    );
    CHECK_FALSE(
      AssetLocator::readText(AssetLocator::assetPath("letters/en.md")).empty()
    );
    CHECK_THROWS_AS(
      AssetLocator::readBytes(AssetLocator::assetPath("missing/asset.png")),
      std::runtime_error
    );
  }
} // namespace wheel_of_fear
