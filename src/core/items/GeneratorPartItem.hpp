#pragma once

#include "core/items/Item.hpp"

namespace wheel_of_fear {
  class GeneratorPartItem: public Item {
    public:
      [[nodiscard]]
      bool isGeneratorPart() const override {
        return true;
      }

    protected:
      static constexpr int part_carry_limit = 9;

      GeneratorPartItem(
        ItemEnum           kind,
        LocalizedTextProps display_name,
        ColorProps         color
      )
          : Item(
              kind,
              display_name,
              color,
              part_carry_limit
            ) {
      }
  };
} // namespace wheel_of_fear
