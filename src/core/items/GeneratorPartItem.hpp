#pragma once

#include "core/items/Item.hpp"

namespace wheel_of_fear {

class GeneratorPartItem : public Item {
public:
    bool isGeneratorPart() const override { return true; }

protected:
    static constexpr int part_carry_limit = 9;

    GeneratorPartItem(ItemEnum kind, std::string_view display_name, Color color) : Item(kind, display_name, color, part_carry_limit) {}
};

}
