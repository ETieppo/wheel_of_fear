#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace wheel_of_fear {

class AssetLocator final {
public:
    AssetLocator() = delete;

    static std::filesystem::path assetPath(std::string_view relative_path);
    static std::filesystem::path shaderPath(std::string_view file_name);
    static std::vector<std::uint8_t> readBytes(const std::filesystem::path& path);
    static std::string readText(const std::filesystem::path& path);

private:
    static std::filesystem::path resolveDirectory(std::string_view bundled_name, std::string_view development_directory);
};

}
