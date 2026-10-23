#pragma once

#include <array>
#include <span>

#include "core/items/Item.hpp"
#include "core/items/ThrowableItem.hpp"

namespace wheel_of_fear {
  class ItemCatalog final {
    public:
      ItemCatalog() = delete;

      static
      const Item &of(ItemEnum kind);
      static
      const ThrowableItem &throwable(ItemEnum kind);
      static
      std::span<const ItemEnum> generatorParts();
      static
      std::span<const ItemEnum> throwables();
  };
} // namespace wheel_of_fear
