#include "core/items/ItemCatalog.hpp"

#include "core/items/FuseItem.hpp"
#include "core/items/GearItem.hpp"
#include "core/items/LureBeaconItem.hpp"
#include "core/items/NeonGrenadeItem.hpp"
#include "core/items/ValveItem.hpp"

namespace wheel_of_fear {
  namespace {
    struct ItemInstancesProps {
        GearItem        gear;
        FuseItem        fuse;
        ValveItem       valve;
        LureBeaconItem  lure_beacon;
        NeonGrenadeItem neon_grenade;
    };

    const ItemInstancesProps &itemInstances() {
      static const ItemInstancesProps instances;
      return instances;
    }

    constexpr std::array<ItemEnum, 3> generator_part_kinds {
      ItemEnum::gear,
      ItemEnum::fuse,
      ItemEnum::valve,
    };
    constexpr std::array<ItemEnum, 2> throwable_kinds {
      ItemEnum::lure_beacon,
      ItemEnum::neon_grenade,
    };
  } // namespace

  const Item &ItemCatalog::of(
    ItemEnum kind
  ) {
    switch(kind) {
      case ItemEnum::gear:         return itemInstances().gear;
      case ItemEnum::fuse:         return itemInstances().fuse;
      case ItemEnum::valve:        return itemInstances().valve;
      case ItemEnum::lure_beacon:  return itemInstances().lure_beacon;
      case ItemEnum::neon_grenade: return itemInstances().neon_grenade;
    }

    return itemInstances().gear;
  }

  const ThrowableItem &ItemCatalog::throwable(
    ItemEnum kind
  ) {
    if(kind == ItemEnum::neon_grenade) return itemInstances().neon_grenade;

    return itemInstances().lure_beacon;
  }

  std::span<const ItemEnum> ItemCatalog::generatorParts() {
    return generator_part_kinds;
  }

  std::span<const ItemEnum> ItemCatalog::throwables() {
    return throwable_kinds;
  }
} // namespace wheel_of_fear
