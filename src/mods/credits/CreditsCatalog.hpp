#pragma once

#include "mods/credits/CreditEntryProps.hpp"
#include <optional>
#include <string_view>
#include <vector>

namespace wheel_of_fear {
  class CreditsCatalog final {
    public:
      CreditsCatalog() = delete;

      static constexpr std::string_view developer_name
        = "Emerson A. Tieppo Jr.";

      static std::vector<CreditEntryProps> load();
      static std::vector<CreditEntryProps> parse(std::string_view markdown);

    private:
      static constexpr std::string_view credits_file_name = "CREDITS.md";
      static constexpr std::string_view header_asset_cell = "Asset";

      static std::optional<CreditEntryProps> parseRow(std::string_view line);
      static std::string_view                trim(std::string_view text);
  };
} // namespace wheel_of_fear
