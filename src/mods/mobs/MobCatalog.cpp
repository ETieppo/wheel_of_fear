#include "mods/mobs/MobCatalog.hpp"

#include "mods/mobs/species/BloaterMob.hpp"
#include "mods/mobs/species/CrawlerMob.hpp"
#include "mods/mobs/species/MelterMob.hpp"
#include "mods/mobs/species/MimicMob.hpp"
#include "mods/mobs/species/MothMob.hpp"
#include "mods/mobs/species/ShadeMob.hpp"
#include "mods/mobs/species/ShepherdMob.hpp"

namespace wheel_of_fear {
  const MobTraitsProps &MobCatalog::traitsOf(
    MobKindEnum kind
  ) {
    switch(kind) {
      case MobKindEnum::crawler:  return CrawlerMob::species_traits;
      case MobKindEnum::bloater:  return BloaterMob::species_traits;
      case MobKindEnum::melter:   return MelterMob::species_traits;
      case MobKindEnum::shepherd: return ShepherdMob::species_traits;
      case MobKindEnum::shade:    return ShadeMob::species_traits;
      case MobKindEnum::mimic:    return MimicMob::species_traits;
      case MobKindEnum::moth:     return MothMob::species_traits;
    }

    return CrawlerMob::species_traits;
  }

  std::unique_ptr<MobEntity> MobCatalog::create(
    MobKindEnum kind,
    Vec2Props   position
  ) {
    switch(kind) {
      case MobKindEnum::crawler: return std::make_unique<CrawlerMob>(position);
      case MobKindEnum::bloater: return std::make_unique<BloaterMob>(position);
      case MobKindEnum::melter:  return std::make_unique<MelterMob>(position);
      case MobKindEnum::shepherd:
        return std::make_unique<ShepherdMob>(position);
      case MobKindEnum::shade: return std::make_unique<ShadeMob>(position);
      case MobKindEnum::mimic: return std::make_unique<MimicMob>(position);
      case MobKindEnum::moth:  return std::make_unique<MothMob>(position);
    }

    return std::make_unique<CrawlerMob>(position);
  }
} // namespace wheel_of_fear
