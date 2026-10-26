#pragma once

#include <cstdint>
#include <span>
#include <string_view>

namespace wheel_of_fear {
  struct EmbeddedAssetProps {
      std::string_view              path;
      std::span<const std::uint8_t> bytes;
  };
} // namespace wheel_of_fear
