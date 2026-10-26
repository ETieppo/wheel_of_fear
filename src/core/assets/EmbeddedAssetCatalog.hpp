#pragma once

#include "core/assets/EmbeddedAssetProps.hpp"
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

namespace wheel_of_fear {
  class EmbeddedAssetCatalog final {
    public:
      EmbeddedAssetCatalog() = delete;

      static std::span<const EmbeddedAssetProps> entries();

      static std::optional<std::span<const std::uint8_t>>
      find(std::string_view path);
  };
} // namespace wheel_of_fear
