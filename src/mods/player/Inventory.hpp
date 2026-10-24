#pragma once

#include <array>

#include "core/items/enums/ItemEnum.hpp"

namespace wheel_of_fear {
  class Inventory {
    public:
      void restock();

      int count(
        ItemEnum item
      ) const {
        return counts_[indexOf(item)];
      }

      bool has(
        ItemEnum item
      ) const {
        return count(item) > 0;
      }

      bool receive(
        ItemEnum item
      );
      bool consume(
        ItemEnum item
      );

    private:
      static constexpr int starting_lures    = 2;
      static constexpr int starting_grenades = 1;

      std::array<int, item_count> counts_ {};
  };
} // namespace wheel_of_fear
