#pragma once

#include <cstdint>
#include <optional>

#include "core/math/RandomGenerator.hpp"
#include "mods/generators/SkillCheckZoneProps.hpp"
#include "mods/generators/enums/SkillCheckResultEnum.hpp"

namespace wheel_of_fear {
  class SkillCheckMinigame {
    public:
      SkillCheckMinigame() = default;
      explicit SkillCheckMinigame(
        const SkillCheckZoneProps &zone
      );

      [[nodiscard]]
      bool isActive() const {
        return is_active_;
      }

      [[nodiscard]]
      bool isWarning() const {
        return warning_seconds_ > 0;
      }

      [[nodiscard]]
      float needleAngle() const {
        return needle_angle_;
      }

      [[nodiscard]]
      float zoneStart() const {
        return zone_start_;
      }

      [[nodiscard]]
      float zoneEnd() const {
        return zone_start_ + zone_width_;
      }

      [[nodiscard]]
      float perfectStart() const {
        return zone_start_ + ((zone_width_ - perfect_width_) / 2);
      }

      [[nodiscard]]
      float perfectWidth() const {
        return perfect_width_;
      }

      [[nodiscard]]
      float resultSeconds() const {
        return result_seconds_;
      }

      [[nodiscard]]
      SkillCheckResultEnum lastResult() const {
        return last_result_;
      }

      [[nodiscard]]
      SkillCheckResultEnum judge() const;
      [[nodiscard]]
      bool hasMissed() const;

      void setNeedleAngle(
        float needle_angle
      ) {
        needle_angle_ = needle_angle;
      }

      void postpone(
        float seconds
      ) {
        countdown_seconds_ = seconds;
      }

      void cancel() {
        is_active_ = false;
      }

      void tickResult(
        float delta_seconds
      );
      bool tickCountdown(
        float delta_seconds
      );
      void begin(
        std::uint32_t    phase,
        RandomGenerator &random
      );
      std::optional<SkillCheckResultEnum> advanceNeedle(
        float delta_seconds,
        bool  is_sync_pressed
      );
      void resolve(
        SkillCheckResultEnum result,
        RandomGenerator     &random
      );

    private:
      static constexpr float warning_duration_seconds = 0.45F;
      static constexpr float result_display_seconds   = 0.7F;

      bool                 is_active_              = false;
      float                warning_seconds_        = 0;
      float                needle_angle_           = 0;
      float                zone_start_             = 0;
      float                zone_width_             = 0.9F;
      float                perfect_width_          = 0.22F;
      float                revolutions_per_second_ = 0.8F;
      float                countdown_seconds_      = 2.5F;
      float                result_seconds_         = 0;
      SkillCheckResultEnum last_result_            = SkillCheckResultEnum::none;
  };
} // namespace wheel_of_fear
