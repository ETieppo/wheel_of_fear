#include "mods/director/demos/DemoScenarioFactory.hpp"
#include "mods/director/demos/DoorsDemoScenario.hpp"
#include "mods/director/demos/FreezeDemoScenario.hpp"
#include "mods/director/demos/GrenadeDemoScenario.hpp"
#include "mods/director/demos/LockerDemoScenario.hpp"
#include "mods/director/demos/LureDemoScenario.hpp"
#include "mods/director/demos/MothDemoScenario.hpp"
#include "mods/director/demos/PatrolDemoScenario.hpp"
#include "mods/director/demos/PowerDemoScenario.hpp"
#include "mods/director/demos/RepairDemoScenario.hpp"
#include "mods/director/demos/SectorsDemoScenario.hpp"
#include "mods/director/demos/ShowcaseDemoScenario.hpp"
#include "mods/director/demos/TrackerDemoScenario.hpp"
#include "mods/director/demos/WalkDemoScenario.hpp"

namespace wheel_of_fear {
  std::unique_ptr<DemoScenario>
  DemoScenarioFactory::create(const DemoModeProps &demo_mode) {
    switch(demo_mode.mode) {
      case DemoModeEnum::none: return nullptr;
      case DemoModeEnum::play:
        return std::make_unique<PatrolDemoScenario>(first_sector);
      case DemoModeEnum::deep:
        return std::make_unique<PatrolDemoScenario>(deep_sector);
      case DemoModeEnum::tracker:
        return std::make_unique<TrackerDemoScenario>();
      case DemoModeEnum::sectors:
        return std::make_unique<SectorsDemoScenario>();
      case DemoModeEnum::showcase:
        return std::make_unique<ShowcaseDemoScenario>();
      case DemoModeEnum::freeze: return std::make_unique<FreezeDemoScenario>();
      case DemoModeEnum::moth:   return std::make_unique<MothDemoScenario>();
      case DemoModeEnum::locker:
        return std::make_unique<LockerDemoScenario>(false);
      case DemoModeEnum::witness:
        return std::make_unique<LockerDemoScenario>(true);
      case DemoModeEnum::repair: return std::make_unique<RepairDemoScenario>();
      case DemoModeEnum::phase:
        return std::make_unique<PatrolDemoScenario>(
          demo_mode.starting_phase.value_or(first_sector)
        );
      case DemoModeEnum::power: return std::make_unique<PowerDemoScenario>();
      case DemoModeEnum::grenade:
        return std::make_unique<GrenadeDemoScenario>();
      case DemoModeEnum::lure:  return std::make_unique<LureDemoScenario>();
      case DemoModeEnum::doors: return std::make_unique<DoorsDemoScenario>();
      case DemoModeEnum::walk:  return std::make_unique<WalkDemoScenario>();
    }

    return nullptr;
  }
} // namespace wheel_of_fear
