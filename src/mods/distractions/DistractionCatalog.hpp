#pragma once

#include "mods/distractions/DistractionTraitsProps.hpp"
#include "mods/distractions/enums/DistractionKindEnum.hpp"

namespace wheel_of_fear {

class DistractionCatalog final {
public:
    DistractionCatalog() = delete;

    static DistractionTraitsProps traitsOf(DistractionKindEnum kind);
};

}
