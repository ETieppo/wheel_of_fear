#pragma once

#include <array>
#include <cstddef>

#include "core/audio/enums/SoundEnum.hpp"
#include "core/entity/Entity.hpp"
#include "core/items/enums/ItemEnum.hpp"
#include "core/math/Color.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/generators/SkillCheck.hpp"
#include "mods/generators/enums/GeneratorIncidentEnum.hpp"
#include "mods/generators/enums/GeneratorStateEnum.hpp"

namespace wheel_of_fear {
  class Generator final: public Entity {
    public:
      static constexpr std::array<float, 2> incident_thresholds {0.34F, 0.68F};
      using PlannedIncidents =
        std::array<GeneratorIncidentEnum, incident_thresholds.size()>;

      Generator(
        Vec2                    position,
        std::size_t             room_index,
        char                    label,
        const PlannedIncidents &planned_incidents
      );

      std::size_t roomIndex() const {
        return room_index_;
      }

      char label() const {
        return label_;
      }

      float progress() const {
        return progress_;
      }

      GeneratorStateEnum state() const {
        return state_;
      }

      bool isOnline() const {
        return state_ == GeneratorStateEnum::online;
      }

      bool isBeingRepaired() const {
        return is_being_repaired_;
      }

      float secondsOnline() const {
        return seconds_online_;
      }

      const SkillCheck &skillCheck() const {
        return skill_check_;
      }

      SkillCheck &skillCheck() {
        return skill_check_;
      }

      Color statusColor() const;

      void update(
        GameContextDto &context,
        float           seconds_to_charge
      );
      void overload(
        GameContextDto &context
      );
      void draw(
        GameContextDto &context
      ) const;
      void forceOnline(
        GameContextDto &context
      );

      void cancelPlannedIncidents() {
        planned_incidents_.fill(GeneratorIncidentEnum::none);
      }

      void setProgress(
        float progress
      ) {
        progress_ = progress;
      }

    private:
      void playSound(
        GameContextDto &context,
        SoundEnum       sound,
        float           volume,
        float           pitch
      ) const;
      void updateCharging(
        GameContextDto &context,
        bool            is_player_near,
        float           seconds_to_charge
      );
      void regressWhileUnattended(
        GameContextDto &context
      );
      void triggerCrossedIncidents(
        GameContextDto &context,
        float           previous_progress
      );
      void updateSkillCheck(
        GameContextDto &context
      );
      void resolveSkillCheck(
        GameContextDto      &context,
        SkillCheckResultEnum result
      );
      void updateJammed(
        GameContextDto &context,
        bool            is_player_near
      );
      void updateBroken(
        GameContextDto &context,
        bool            is_player_near
      );
      void updateOnline(
        GameContextDto &context
      );
      void triggerIncident(
        GameContextDto       &context,
        GeneratorIncidentEnum incident
      );
      void bringOnline(
        GameContextDto &context
      );
      void drawProgressRing(
        GameContextDto &context,
        Color           color
      ) const;

      std::size_t                                  room_index_;
      char                                         label_;
      PlannedIncidents                             planned_incidents_;
      std::array<bool, incident_thresholds.size()> triggered_incidents_ {};
      float                                        progress_ = 0;
      GeneratorStateEnum state_                   = GeneratorStateEnum::offline;
      ItemEnum           required_item_           = ItemEnum::gear;
      float              seconds_unattended_      = 0;
      float              ratchet_timer_           = 0;
      float              rotor_angle_             = 0;
      float              rotor_speed_             = 0;
      float              spark_timer_             = 0;
      float              wiring_progress_         = 0;
      float              repair_sound_timer_      = 0;
      bool               is_being_repaired_       = false;
      bool               was_repaired_last_frame_ = false;
      float              seconds_online_          = 0;
      SkillCheck         skill_check_;
      bool               has_warned_regression_ = false;
  };
} // namespace wheel_of_fear
