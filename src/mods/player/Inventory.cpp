#include "mods/player/Inventory.hpp"

#include "core/items/ItemCatalog.hpp"

namespace wheel_of_fear {
  void Inventory::restock() {
    counts_.fill(0);
    counts_[indexOf(ItemEnum::lure_beacon)]  = starting_lures;
    counts_[indexOf(ItemEnum::neon_grenade)] = starting_grenades;
  }

  bool Inventory::receive(ItemEnum item) {
    int &slot = counts_[indexOf(item)];
    if(slot >= ItemCatalog::of(item).carryLimit()) return false;
    slot += 1;
    return true;
  }

  bool Inventory::consume(ItemEnum item) {
    if(!has(item)) return false;
    counts_[indexOf(item)] -= 1;
    return true;
  }
} // namespace wheel_of_fear
