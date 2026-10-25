#include "mods/mobs/MobSystem.hpp"
#include "core/render/Renderer.hpp"
#include "core/time/GameClock.hpp"
#include "mods/mobs/MobBodyCollisionResolver.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>

namespace wheel_of_fear {
  MobSystem::MobSystem()
      : spawner_(mobs_)
      , danger_(mobs_)
      , hugs_(mobs_)
      , explosions_(mobs_) {
  }

  MobSystem::~MobSystem() = default;

  void MobSystem::populatePhase(WorldSystem &world) {
    mobs_.clear();
    grace_seconds_ = phase_grace_seconds;
    danger_.reset();
    spawner_.populatePhase(world);
  }

  void MobSystem::onGeneratorOnline(GameContextDto &context) {
    for(const auto &mob : mobs_) mob->onGeneratorOnline(context);
  }

  void MobSystem::answerShepherd(
    Vec2Props caller,
    Vec2Props target,
    float     radius,
    float     frenzy_seconds
  ) {
    for(const auto &mob : mobs_) {
      if(
        !mob->isAlive()
        || !mob->traits().is_hostile
        || mob->kind()
        == MobKindEnum::shepherd
        || mob->isDisguised()
      )
        continue;

      if(mob->distanceTo(caller) > radius) continue;
      mob->condition().frenzy(frenzy_seconds);
      mob->perception().hearShepherd(target);
    }
  }

  void MobSystem::update(GameContextDto &context) {
    if(!context.clock.isRunning()) return;

    const float delta = context.clock.deltaSeconds();
    grace_seconds_    = std::max(0.0F, grace_seconds_ - delta);

    for(const auto &mob : mobs_)
      if(mob->isAlive()) mob->emitLights(context);
    for(const auto &mob : mobs_) updateConditions(context, *mob, delta);

    if(const auto locker_index = context.player.hiding().hideEventLockerIndex())
      witnessHiding(context, *locker_index);
    for(const auto &mob : mobs_) mob->update(context);
    MobBodyCollisionResolver::resolve(
      mobs_,
      context.player,
      context.world.map()
    );

    std::erase_if(mobs_, [](const auto &mob) { return !mob->isAlive(); });
    spawner_.update(context);
    danger_.update(context);
  }

  void MobSystem::updateConditions(
    GameContextDto &context,
    MobEntity      &mob,
    float           delta_seconds
  ) const {
    const TileMapModel &map = context.world.map();
    if(!mob.traits().passes_through_walls && map.isSolidAt(mob.position()))
      mob.setPosition(map.nearestFloor(mob.position()));

    MobConditionComponent &condition  = mob.condition();
    const bool             was_frozen = condition.isFrozen();

    condition.setFrozen(condition.isHeldByLight(context));
    if(was_frozen && !condition.isFrozen() && !mob.isBusy())
      mob.onLightReleased(context);

    condition.calmFrenzy(delta_seconds);
    if(mob.traits().is_hostile && !mob.isDisguised())
      mob.perception().perceive(context, grace_seconds_ <= 0);

    condition.advanceBurning(context);
    condition.advanceHugAndCrumble(context);
  }

  void MobSystem::witnessHiding(
    const GameContextDto &context,
    std::size_t           locker_index
  ) {
    const Vec2Props locker_position
      = context.world.lockers()[locker_index].position();

    for(const auto &mob : mobs_)
      mob->perception().observeHiding(context, locker_index, locker_position);
  }

  void MobSystem::draw(GameContextDto &context) {
    if(context.world.map().width() == 0) return;

    for(const auto &mob : mobs_) {
      if(!context.renderer.isVisible(mob->position(), 30)) continue;

      mob->appearance().refreshVisibility(context);
      mob->draw(context);
    }
  }
} // namespace wheel_of_fear
