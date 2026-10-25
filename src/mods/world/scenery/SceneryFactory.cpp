#include "mods/world/scenery/SceneryFactory.hpp"
#include "mods/world/scenery/BarrelScenery.hpp"
#include "mods/world/scenery/CrateScenery.hpp"
#include "mods/world/scenery/JunctionBoxScenery.hpp"
#include "mods/world/scenery/PipesScenery.hpp"
#include "mods/world/scenery/ServerRackScenery.hpp"
#include "mods/world/scenery/TerminalScenery.hpp"
#include "mods/world/scenery/TreeScenery.hpp"

namespace wheel_of_fear {
  std::unique_ptr<Scenery> SceneryFactory::create(
    SceneryKindEnum            kind,
    const SceneryPlacementDto &placement
  ) {
    switch(kind) {
      case SceneryKindEnum::barrel:
        return std::make_unique<BarrelScenery>(placement);

      case SceneryKindEnum::crate:
        return std::make_unique<CrateScenery>(placement);

      case SceneryKindEnum::pipes:
        return std::make_unique<PipesScenery>(placement);

      case SceneryKindEnum::junction_box:
        return std::make_unique<JunctionBoxScenery>(placement);

      case SceneryKindEnum::server_rack:
        return std::make_unique<ServerRackScenery>(placement);

      case SceneryKindEnum::terminal:
        return std::make_unique<TerminalScenery>(placement);

      case SceneryKindEnum::tree:
        return std::make_unique<TreeScenery>(placement);
    }

    return std::make_unique<BarrelScenery>(placement);
  }
} // namespace wheel_of_fear
