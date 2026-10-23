#include "mods/mobs/MobCatalog.hpp"

#include "mods/mobs/species/Bloater.hpp"
#include "mods/mobs/species/Crawler.hpp"
#include "mods/mobs/species/Melter.hpp"
#include "mods/mobs/species/Mimic.hpp"
#include "mods/mobs/species/Moth.hpp"
#include "mods/mobs/species/Shade.hpp"
#include "mods/mobs/species/Shepherd.hpp"

namespace wheel_of_fear {
  const MobTraitsProps &MobCatalog::traitsOf(MobKindEnum kind) {
    switch(kind) {
      case MobKindEnum::crawler:  return Crawler::species_traits;
      case MobKindEnum::bloater:  return Bloater::species_traits;
      case MobKindEnum::melter:   return Melter::species_traits;
      case MobKindEnum::shepherd: return Shepherd::species_traits;
      case MobKindEnum::shade:    return Shade::species_traits;
      case MobKindEnum::mimic:    return Mimic::species_traits;
      case MobKindEnum::moth:     return Moth::species_traits;
    }
    return Crawler::species_traits;
  }

  std::unique_ptr<Mob> MobCatalog::create(
    MobKindEnum kind,
    Vec2        position
  ) {
    switch(kind) {
      case MobKindEnum::crawler:  return std::make_unique<Crawler>(position);
      case MobKindEnum::bloater:  return std::make_unique<Bloater>(position);
      case MobKindEnum::melter:   return std::make_unique<Melter>(position);
      case MobKindEnum::shepherd: return std::make_unique<Shepherd>(position);
      case MobKindEnum::shade:    return std::make_unique<Shade>(position);
      case MobKindEnum::mimic:    return std::make_unique<Mimic>(position);
      case MobKindEnum::moth:     return std::make_unique<Moth>(position);
    }
    return std::make_unique<Crawler>(position);
  }
} // namespace wheel_of_fear
