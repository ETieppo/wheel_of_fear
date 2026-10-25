#include "mods/credits/CreditsCatalog.hpp"
#include "core/assets/AssetLocator.hpp"
#include <SDL3/SDL_log.h>
#include <array>
#include <exception>
#include <string>

namespace wheel_of_fear {
  std::vector<CreditEntryProps> CreditsCatalog::load() {
    try {
      return parse(
        AssetLocator::readText(AssetLocator::assetPath(credits_file_name))
      );
    } catch(const std::exception &error) {
      SDL_Log("credits unavailable: %s", error.what());
      return {};
    }
  }

  std::vector<CreditEntryProps>
  CreditsCatalog::parse(std::string_view markdown) {
    std::vector<CreditEntryProps> entries;

    while(!markdown.empty()) {
      const std::size_t      line_end = markdown.find('\n');
      const std::string_view line     = markdown.substr(0, line_end);
      markdown                        = line_end == std::string_view::npos
        ? std::string_view {}
        : markdown.substr(line_end + 1);

      if(const std::optional<CreditEntryProps> entry = parseRow(line))
        entries.push_back(*entry);
    }

    return entries;
  }

  std::optional<CreditEntryProps>
  CreditsCatalog::parseRow(std::string_view line) {
    line = trim(line);
    if(!line.starts_with('|')) return std::nullopt;

    constexpr std::size_t           required_cell_count = 3;
    std::array<std::string_view, 3> cells {};
    std::size_t                     cell_count = 0;
    std::string_view                remaining  = line.substr(1);

    while(cell_count < required_cell_count) {
      const std::size_t separator = remaining.find('|');
      if(separator == std::string_view::npos) return std::nullopt;

      cells[cell_count++] = trim(remaining.substr(0, separator));
      remaining           = remaining.substr(separator + 1);
    }

    const auto &[asset, author, license] = cells;
    if(asset.empty() || asset == header_asset_cell || asset.starts_with('-'))
      return std::nullopt;

    return CreditEntryProps {
      .asset   = std::string(asset),
      .author  = std::string(author),
      .license = std::string(license),
    };
  }

  std::string_view CreditsCatalog::trim(std::string_view text) {
    constexpr std::string_view whitespace = " \t\r";
    const std::size_t          first      = text.find_first_not_of(whitespace);
    if(first == std::string_view::npos) return {};

    const std::size_t last = text.find_last_not_of(whitespace);
    return text.substr(first, last - first + 1);
  }
} // namespace wheel_of_fear
