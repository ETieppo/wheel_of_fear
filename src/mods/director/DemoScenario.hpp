#pragma once

#include <cstddef>
#include <cstdint>

#include "core/input/enums/InputActionEnum.hpp"
#include "core/math/Vec2.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/director/enums/DemoModeEnum.hpp"
#include "mods/mobs/enums/MobKindEnum.hpp"

namespace wheel_of_fear {
  class DemoScenario {
    public:
      DemoScenario();

      bool isActive() const {
        return mode_ != DemoModeEnum::none;
      }

      std::uint32_t startingPhase() const;
      void          restart();
      void          run(
        GameContextDto &context,
        float           seconds
      );

    private:
      static constexpr float         log_interval_seconds = 0.5F;
      static constexpr std::uint32_t deep_sector          = 6;

      static
      void logPatrols(
        GameContextDto &context,
        float           seconds
      );
      static
      void spawnInSight(
        GameContextDto &context,
        MobKindEnum     kind,
        float           distance
      );
      static
      void aimAtWorldOffset(
        GameContextDto &context,
        Vec2            offset
      );

      void runSectors(GameContextDto &context);
      void runShowcase(GameContextDto &context);
      void runFreeze(
        GameContextDto &context,
        float           seconds,
        bool            should_log
      );
      void runMoth(
        GameContextDto &context,
        bool            should_log
      );
      void runLocker(
        GameContextDto &context,
        float           seconds,
        bool            should_log,
        bool            has_witness
      );
      void runRepair(
        GameContextDto &context,
        float           seconds,
        bool            should_log
      );
      void runPower(
        GameContextDto &context,
        bool            should_log
      );
      void runGrenade(
        GameContextDto &context,
        float           seconds,
        bool            should_log
      );
      void runLure(
        GameContextDto &context,
        float           seconds,
        bool            should_log
      );
      void runDoors(
        GameContextDto &context,
        bool            should_log
      );
      void runWalk(
        GameContextDto &context,
        float           seconds,
        bool            should_log
      );

      DemoModeEnum  mode_;
      std::uint32_t step_                 = 0;
      float         log_timer_            = 0;
      std::size_t   total_moved_doorways_ = 0;
      std::uint32_t last_phase_           = 0;
  };
} // namespace wheel_of_fear
