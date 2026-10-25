#include "mods/credits/CreditsCatalog.hpp"
#include <doctest/doctest.h>
#include <string_view>

namespace wheel_of_fear {
  TEST_CASE("credits catalog reads asset rows from the markdown table") {
    constexpr std::string_view markdown
      = "# Asset credits\n"
        "\n"
        "| Asset | Author | License | Source |\n"
        "|---|---|---|---|\n"
        "| impact | Kenney | CC0 | https://kenney.nl/assets/impact-sounds |\n"
        "| survivor | Riley Gombart | CC-BY 3.0 | https://example.org |\n";

    const auto entries = CreditsCatalog::parse(markdown);

    REQUIRE(entries.size() == 2);
    CHECK(entries[0].asset == "impact");
    CHECK(entries[0].author == "Kenney");
    CHECK(entries[1].license == "CC-BY 3.0");
  }

  TEST_CASE("credits catalog loads the bundled credits file") {
    const auto entries = CreditsCatalog::load();

    REQUIRE_FALSE(entries.empty());
    CHECK_FALSE(entries.front().author.empty());
  }
} // namespace wheel_of_fear
