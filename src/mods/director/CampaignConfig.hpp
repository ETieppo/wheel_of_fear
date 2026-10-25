#pragma once

#include <algorithm>
#include <cstdint>

namespace wheel_of_fear {
  class CampaignConfig final {
    public:
      CampaignConfig() = delete;

      static constexpr std::uint32_t first_phase = 1;
      static constexpr std::uint32_t final_phase = 12;

      [[nodiscard]]
      static constexpr std::uint32_t clampPhase(std::uint32_t phase) {
        return std::clamp(phase, first_phase, final_phase);
      }
  };
} // namespace wheel_of_fear
