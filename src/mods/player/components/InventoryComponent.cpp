#include "mods/player/components/InventoryComponent.hpp"

#include "core/items/ItemCatalog.hpp"

namespace wheel_of_fear {
  void InventoryComponent::restock() {
    counts_.fill(0);
    counts_[indexOf(ItemEnum::lure_beacon)]  = starting_lures;
    counts_[indexOf(ItemEnum::neon_grenade)] = starting_grenades;
  }

  bool InventoryComponent::receive(
    ItemEnum item
  ) {
    int &slot = counts_[indexOf(item)];
    if(slot >= ItemCatalog::of(item).carryLimit()) return false;

    slot += 1;

    return true;
  }

  bool InventoryComponent::consume(
    ItemEnum item
  ) {
    if(!has(item)) return false;

    counts_[indexOf(item)] -= 1;

    return true;
  }
} // namespace wheel_of_fear
