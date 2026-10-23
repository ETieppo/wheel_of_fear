#pragma once

#include <memory>

#include "mods/world/dtos/SceneryPlacementDto.hpp"
#include "mods/world/enums/SceneryKindEnum.hpp"
#include "mods/world/scenery/Scenery.hpp"

namespace wheel_of_fear {

class SceneryFactory final {
public:
    SceneryFactory() = delete;

    static std::unique_ptr<Scenery> create(SceneryKindEnum kind, const SceneryPlacementDto& placement);
};

}
