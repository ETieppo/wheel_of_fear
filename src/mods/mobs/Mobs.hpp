#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

#include "core/math/Vec2.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/mobs/Mob.hpp"
#include "mods/mobs/enums/MobKindEnum.hpp"

namespace wheel_of_fear {
  class Mobs {
    public:
      Mobs();
      ~Mobs();

      const std::vector<std::unique_ptr<Mob>> &all() const {
        return mobs_;
      }

      float dangerLevel() const {
        return danger_level_;
      }

      void setGraceSeconds(float seconds) {
        grace_seconds_ = seconds;
      }

      void clear() {
        mobs_.clear();
      }

      void populatePhase(World &world);
      void spawn(
        MobKindEnum kind,
        Vec2        position
      );
      bool isHugInProgress() const;
      bool tryStartHug(
        Vec2 position,
        Vec2 facing
      );
      void cancelHug();
      void onGeneratorOnline(GameContextDto &context);
      void answerShepherd(
        Vec2  caller,
        Vec2  target,
        float radius,
        float frenzy_seconds
      );
      std::size_t destroyInBlast(
        GameContextDto &context,
        Vec2            center,
        float           radius
      );
      void detonate(
        GameContextDto &context,
        Mob            &bomb
      );
      void update(GameContextDto &context);
      void draw(GameContextDto &context);

      static
      std::vector<std::string_view> introductionsForPhase(std::uint32_t phase);

    private:
      static
      std::optional<MobKindEnum> pickSpawnKind(World &world);
      bool                       isHeldByLight(
        const Mob            &mob,
        const GameContextDto &context
      ) const;
      void witnessHiding(
        const GameContextDto &context,
        std::size_t           locker_index
      );
      void updateSpawner(GameContextDto &context);
      void updateDanger(GameContextDto &context);
      static
      float visibilityOf(
        const Mob            &mob,
        const GameContextDto &context
      );

      std::vector<std::unique_ptr<Mob>> mobs_;
      float                             spawn_timer_     = 0;
      float                             grace_seconds_   = 0;
      float                             danger_level_    = 0;
      float                             heartbeat_timer_ = 0;
  };
} // namespace wheel_of_fear
