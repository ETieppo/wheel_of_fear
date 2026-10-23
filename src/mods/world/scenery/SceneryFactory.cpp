#include "mods/world/scenery/SceneryFactory.hpp"

#include "mods/world/scenery/Barrel.hpp"
#include "mods/world/scenery/Crate.hpp"
#include "mods/world/scenery/JunctionBox.hpp"
#include "mods/world/scenery/NeonSign.hpp"
#include "mods/world/scenery/Pipes.hpp"
#include "mods/world/scenery/ServerRack.hpp"
#include "mods/world/scenery/Terminal.hpp"

namespace wheel_of_fear {

std::unique_ptr<Scenery> SceneryFactory::create(SceneryKindEnum kind, const SceneryPlacementDto& placement) {
    switch (kind) {
    case SceneryKindEnum::barrel: return std::make_unique<Barrel>(placement);
    case SceneryKindEnum::crate: return std::make_unique<Crate>(placement);
    case SceneryKindEnum::pipes: return std::make_unique<Pipes>(placement);
    case SceneryKindEnum::junction_box: return std::make_unique<JunctionBox>(placement);
    case SceneryKindEnum::server_rack: return std::make_unique<ServerRack>(placement);
    case SceneryKindEnum::terminal: return std::make_unique<Terminal>(placement);
    case SceneryKindEnum::neon_sign_arrow: return std::make_unique<NeonSign>(placement, 0);
    case SceneryKindEnum::neon_sign_eye: return std::make_unique<NeonSign>(placement, 1);
    case SceneryKindEnum::neon_sign_bars: return std::make_unique<NeonSign>(placement, 2);
    }
    return std::make_unique<Barrel>(placement);
}

}
