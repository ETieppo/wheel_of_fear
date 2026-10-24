#pragma once

#include "core/audio/enums/SoundEnum.hpp"
#include "core/entity/Entity.hpp"
#include "core/math/ColorProps.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/generators/GeneratorMachineryProps.hpp"
#include "mods/generators/SkillCheckMinigame.hpp"
#include "mods/generators/enums/GeneratorIncidentEnum.hpp"
#include "mods/generators/enums/GeneratorStateEnum.hpp"
#include "mods/generators/states/GeneratorBrokenState.hpp"
#include "mods/generators/states/GeneratorChargingState.hpp"
#include "mods/generators/states/GeneratorJammedState.hpp"
#include "mods/generators/states/GeneratorOnlineState.hpp"
#include <array>
#include <cstddef>

namespace wheel_of_fear {
  class GeneratorEntity final: public Entity {
    public:
      static constexpr std::array<float, 2> incident_thresholds {0.34F, 0.68F};
      static constexpr float                repair_noise_radius  = 230;
      static constexpr float                failure_noise_radius = 430;

      using PlannedIncidents
        = std::array<GeneratorIncidentEnum, incident_thresholds.size()>;

      GeneratorEntity(
        Vec2Props               position,
        std::size_t             room_index,
        char                    label,
        const PlannedIncidents &planned_incidents
      );

      [[nodiscard]]
      std::size_t roomIndex() const {
        return room_index_;
      }

      [[nodiscard]]
      char label() const {
        return label_;
      }

      [[nodiscard]]
      float progress() const {
        return progress_;
      }

      void setProgress(float progress) {
        progress_ = progress;
      }

      [[nodiscard]]
      GeneratorStateEnum state() const {
        return state_;
      }

      [[nodiscard]]
      bool isOnline() const {
        return state_ == GeneratorStateEnum::online;
      }

      [[nodiscard]]
      bool isBeingRepaired() const {
        return is_being_repaired_;
      }

      [[nodiscard]]
      bool wasRepairedLastFrame() const {
        return was_repaired_last_frame_;
      }

      [[nodiscard]]
      float secondsOnline() const {
        return online_state_.secondsOnline();
      }

      [[nodiscard]]
      const SkillCheckMinigame &skillCheck() const {
        return skill_check_;
      }

      [[nodiscard]]
      SkillCheckMinigame &skillCheck() {
        return skill_check_;
      }

      [[nodiscard]]
      GeneratorMachineryProps &machinery() {
        return machinery_;
      }

      [[nodiscard]]
      ColorProps statusColor() const {
        return currentState().statusColor(*this);
      }

      void cancelPlannedIncidents() {
        planned_incidents_.fill(GeneratorIncidentEnum::none);
      }

      void update(
        GameContextDto &context,
        float           seconds_to_charge
      );

      void draw(GameContextDto &context) const;

      void playSound(
        GameContextDto &context,
        SoundEnum       sound,
        float           volume,
        float           pitch
      ) const;

      void enterCharging() {
        state_ = GeneratorStateEnum::charging;
      }

      void triggerIncident(
        GameContextDto       &context,
        GeneratorIncidentEnum incident
      );

      void triggerCrossedIncidents(
        GameContextDto &context,
        float           previous_progress
      );

      void bringOnline(GameContextDto &context);

      void forceOnline(GameContextDto &context);

    private:
      static constexpr float interaction_distance = 36;
      static constexpr int   progress_ring_dots   = 24;
      static constexpr float progress_ring_radius = 23;

      [[nodiscard]]
      const GeneratorState &currentState() const;

      [[nodiscard]]
      GeneratorState &currentState();

      void refreshRepairStatus(
        const GameContextDto &context,
        bool                  is_player_near
      );

      void emitChargeLight(GameContextDto &context) const;

      void drawProgressRing(
        GameContextDto &context,
        ColorProps      color
      ) const;

      std::size_t                                  room_index_;
      std::array<bool, incident_thresholds.size()> triggered_incidents_ {};
      char                                         label_;
      PlannedIncidents                             planned_incidents_;
      float                                        progress_ = 0;
      GeneratorStateEnum      state_             = GeneratorStateEnum::offline;
      GeneratorMachineryProps machinery_         = {};
      bool                    is_being_repaired_ = false;
      bool                    was_repaired_last_frame_ = false;
      SkillCheckMinigame      skill_check_;
      GeneratorChargingState  charging_state_;
      GeneratorJammedState    jammed_state_;
      GeneratorBrokenState    broken_state_;
      GeneratorOnlineState    online_state_;
  };
} // namespace wheel_of_fear
