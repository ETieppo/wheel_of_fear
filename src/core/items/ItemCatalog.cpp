#include "core/items/ItemCatalog.hpp"

#include "core/items/FuseItem.hpp"
#include "core/items/GearItem.hpp"
#include "core/items/LureBeaconItem.hpp"
#include "core/items/NeonGrenadeItem.hpp"
#include "core/items/ValveItem.hpp"

namespace wheel_of_fear {
  namespace {
    const GearItem        gear_item;
    const FuseItem        fuse_item;
    const ValveItem       valve_item;
    const LureBeaconItem  lure_beacon_item;
    const NeonGrenadeItem neon_grenade_item;

    constexpr std::array<ItemEnum, 3> generator_part_kinds {
      ItemEnum::gear, ItemEnum::fuse, ItemEnum::valve
    };
    constexpr std::array<ItemEnum, 2> throwable_kinds {
      ItemEnum::lure_beacon, ItemEnum::neon_grenade
    };
  } // namespace

  const Item &ItemCatalog::of(ItemEnum kind) {
    switch(kind) {
      case ItemEnum::gear:         return gear_item;
      case ItemEnum::fuse:         return fuse_item;
      case ItemEnum::valve:        return valve_item;
      case ItemEnum::lure_beacon:  return lure_beacon_item;
      case ItemEnum::neon_grenade: return neon_grenade_item;
    }
    return gear_item;
  }

  const ThrowableItem &ItemCatalog::throwable(ItemEnum kind) {
    if(kind == ItemEnum::neon_grenade) return neon_grenade_item;
    return lure_beacon_item;
  }

  std::span<const ItemEnum> ItemCatalog::generatorParts() {
    return generator_part_kinds;
  }

  std::span<const ItemEnum> ItemCatalog::throwables() {
    return throwable_kinds;
  }
} // namespace wheel_of_fear
