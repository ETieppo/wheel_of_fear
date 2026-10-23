#pragma once

#include <string_view>

#include "core/items/enums/ItemEnum.hpp"
#include "core/math/Color.hpp"

namespace wheel_of_fear {

class Item {
public:
    virtual ~Item() = default;

    ItemEnum kind() const { return kind_; }
    std::string_view displayName() const { return display_name_; }
    Color color() const { return color_; }
    int carryLimit() const { return carry_limit_; }

    virtual bool isGeneratorPart() const { return false; }
    virtual bool isThrowable() const { return false; }

protected:
    Item(ItemEnum kind, std::string_view display_name, Color color, int carry_limit)
        : kind_(kind), display_name_(display_name), color_(color), carry_limit_(carry_limit) {}

private:
    ItemEnum kind_;
    std::string_view display_name_;
    Color color_;
    int carry_limit_;
};

}
