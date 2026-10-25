#pragma once

#include "core/math/Vec2Props.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/mobs/MobDangerMonitor.hpp"
#include "mods/mobs/MobEntity.hpp"
#include "mods/mobs/MobExplosionHandler.hpp"
#include "mods/mobs/MobHugCoordinator.hpp"
#include "mods/mobs/MobSpawner.hpp"
#include "mods/mobs/enums/MobKindEnum.hpp"
#include <memory>
#include <vector>

namespace wheel_of_fear {
  class MobSystem {
    public:
      MobSystem();
      ~MobSystem();

      MobSystem(const MobSystem &) = delete;

      MobSystem &operator =(const MobSystem &) = delete;

      [[nodiscard]]
      const std::vector<std::unique_ptr<MobEntity>> &all() const {
        return mobs_;
      }

      [[nodiscard]]
      const MobEntity* visibleMobAt(Vec2Props point) const;

      [[nodiscard]]
      const MobDangerMonitor &danger() const {
        return danger_;
      }

      [[nodiscard]]
      MobHugCoordinator &hugs() {
        return hugs_;
      }

      [[nodiscard]]
      MobExplosionHandler &explosions() {
        return explosions_;
      }

      void setGraceSeconds(float seconds) {
        grace_seconds_ = seconds;
      }

      void clear() {
        mobs_.clear();
      }

      void spawn(
        MobKindEnum kind,
        Vec2Props   position
      ) {
        spawner_.spawn(kind, position);
      }

      void populatePhase(WorldSystem &world);

      void onGeneratorOnline(GameContextDto &context);

      void answerCall(
        MobKindEnum caller_kind,
        Vec2Props   caller,
        Vec2Props   target,
        float       radius,
        float       frenzy_seconds
      );

      void update(GameContextDto &context);

      void draw(GameContextDto &context);

    private:
      static constexpr float inspect_margin       = 6;
      static constexpr float inspect_min_presence = 0.35F;

      static constexpr float phase_grace_seconds = 8;

      void updateConditions(
        GameContextDto &context,
        MobEntity      &mob,
        float           delta_seconds
      ) const;

      void witnessHiding(
        const GameContextDto &context,
        std::size_t           locker_index
      );

      std::vector<std::unique_ptr<MobEntity>> mobs_;
      MobSpawner                              spawner_;
      MobDangerMonitor                        danger_;
      MobHugCoordinator                       hugs_;
      MobExplosionHandler                     explosions_;
      float                                   grace_seconds_ = 0;
  };
} // namespace wheel_of_fear
