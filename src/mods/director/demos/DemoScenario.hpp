#pragma once

#include "core/math/Vec2Props.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/director/dtos/DemoTickDto.hpp"
#include "mods/mobs/enums/MobKindEnum.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

namespace wheel_of_fear {
  class DemoScenario {
    public:
      DemoScenario()                                 = default;
      DemoScenario(const DemoScenario &)             = delete;
      DemoScenario &operator =(const DemoScenario &) = delete;
      virtual ~DemoScenario()                        = default;

      [[nodiscard]]
      virtual std::uint32_t startingPhase() const {
        return 1;
      }

      [[nodiscard]]
      virtual bool startsAtEnding() const {
        return false;
      }

      void restart() {
        step_ = 0;
      }

      virtual void run(
        GameContextDto    &context,
        const DemoTickDto &tick
      ) = 0;

    protected:
      static void        logLine(const std::string &line);
      static std::string optionalIndex(std::optional<std::size_t> value);

      static void spawnInSight(
        GameContextDto &context,
        MobKindEnum     kind,
        float           distance
      );

      static void aimAtWorldOffset(
        GameContextDto &context,
        Vec2Props       offset
      );

      [[nodiscard]]
      std::uint32_t step() const {
        return step_;
      }

      void setStep(std::uint32_t step) {
        step_ = step;
      }

    private:
      std::uint32_t step_ = 0;
  };
} // namespace wheel_of_fear
