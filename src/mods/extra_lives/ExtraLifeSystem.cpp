#include "mods/extra_lives/ExtraLifeSystem.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/math/RandomGenerator.hpp"
#include "core/text/Localization.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/hud/HudSystem.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <format>

namespace wheel_of_fear {
  namespace {
    constexpr LocalizedTextProps extra_life_message {
      "EXTRA LIFE  ({}/{})",
      "VIDA EXTRA  ({}/{})",
    };
  } // namespace

  void ExtraLifeSystem::generatePhase(
    WorldSystem  &world,
    std::uint64_t phase_seed,
    bool          can_grant_life
  ) {
    pickup_.reset();

    const auto &rooms = world.map().rooms();
    if(!can_grant_life || rooms.empty()) return;

    RandomGenerator random(phase_seed ^ spawn_seed_salt);
    if(!random.chance(spawn_chance)) return;

    const RoomProps &room = rooms[random.index(rooms.size())];
    pickup_.emplace(world.map().randomFloorInRoom(random, room));
  }

  void ExtraLifeSystem::update(GameContextDto &context) {
    if(!pickup_) return;

    pickup_->emitLight(context.lighting, context.clock.realSeconds());
    if(!context.clock.isRunning()) return;

    const TileMapModel &map = context.world.map();
    if(map.isSolidAt(pickup_->position()))
      pickup_->settleOn(map.nearestFloor(pickup_->position()));

    collect(context);
  }

  void ExtraLifeSystem::collect(GameContextDto &context) {
    PlayerEntity &player = context.player;

    if(
      !player.canBeTargeted()
      || pickup_->position().distance(player.position())
      >= ExtraLifePickupEntity::reach
      || !player.vitality().grantExtraLife()
    )
      return;

    pickup_.reset();
    context.sound_player.play(SoundEnum::part_pickup, 0.8F, 0, 0.8F);

    context.hud.announce(
      Palette::accent,
      Localization::format(
        extra_life_message,
        player.vitality().maxHealth(),
        VitalityComponent::highest_max_health
      )
    );
  }

  void ExtraLifeSystem::draw(GameContextDto &context) const {
    if(!pickup_ || !context.renderer.isVisible(pickup_->position(), 16)) return;

    pickup_->draw(
      context.renderer,
      context.art.sprites(),
      context.clock.realSeconds()
    );
  }
} // namespace wheel_of_fear
