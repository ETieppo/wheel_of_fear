#pragma once

#include "core/items/enums/ItemEnum.hpp"
#include "core/math/ColorProps.hpp"
#include <string_view>

namespace wheel_of_fear {
  class Item {
    public:
      virtual ~Item() = default;

      [[nodiscard]]
      ItemEnum kind() const {
        return kind_;
      }

      [[nodiscard]]
      std::string_view displayName() const {
        return display_name_;
      }

      [[nodiscard]]
      ColorProps color() const {
        return color_;
      }

      [[nodiscard]]
      int carryLimit() const {
        return carry_limit_;
      }

      [[nodiscard]]
      virtual bool isGeneratorPart() const {
        return false;
      }

      [[nodiscard]]
      virtual bool isThrowable() const {
        return false;
      }

    protected:
      Item(
        ItemEnum         kind,
        std::string_view display_name,
        ColorProps       color,
        int              carry_limit
      )
          : kind_(kind)
          , display_name_(display_name)
          , color_(color)
          , carry_limit_(carry_limit) {
      }

    private:
      ItemEnum         kind_;
      std::string_view display_name_;
      ColorProps       color_;
      int              carry_limit_;
  };
} // namespace wheel_of_fear
