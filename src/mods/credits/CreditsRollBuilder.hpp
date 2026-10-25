#pragma once

#include "mods/credits/CreditEntryProps.hpp"
#include "mods/credits/CreditLineProps.hpp"
#include <span>
#include <vector>

namespace wheel_of_fear {
  class CreditsRollBuilder final {
    public:
      CreditsRollBuilder() = delete;

      static std::vector<CreditLineProps>
      build(std::span<const CreditEntryProps> entries);

      [[nodiscard]]
      static float totalHeight(std::span<const CreditLineProps> lines);
  };
} // namespace wheel_of_fear
