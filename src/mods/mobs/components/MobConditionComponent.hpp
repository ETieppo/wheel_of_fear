#pragma once

#include "game/dtos/GameContextDto.hpp"

namespace wheel_of_fear {
  class MobEntity;

  class MobConditionComponent {
    public:
      void startBurning();

      explicit MobConditionComponent(MobEntity &owner)
          : owner_(owner) {
      }

      [[nodiscard]]
      bool isFrozen() const {
        return is_frozen_;
      }

      void setFrozen(bool is_frozen) {
        is_frozen_ = is_frozen;
      }

      [[nodiscard]]
      bool isBeingHugged() const {
        return is_being_hugged_;
      }

      void setBeingHugged(bool is_being_hugged) {
        is_being_hugged_ = is_being_hugged;
      }

      [[nodiscard]]
      bool isCrumbling() const {
        return crumble_seconds_ > 0;
      }

      [[nodiscard]]
      bool isBurning() const {
        return burn_seconds_ > 0;
      }

      [[nodiscard]]
      float frenzySeconds() const {
        return frenzy_seconds_;
      }

      [[nodiscard]]
      float petrifyAmount() const {
        return petrify_amount_;
      }

      void frenzy(float seconds);

      void calmFrenzy(float delta_seconds);

      [[nodiscard]]
      bool isHeldByLight(const GameContextDto &context) const;

      void advanceBurning(GameContextDto &context);

      void advanceHugAndCrumble(GameContextDto &context);

    private:
      static constexpr float freeze_threshold            = 0.035F;
      static constexpr float burn_duration_seconds       = 0.9F;
      static constexpr float hug_break_distance          = 28;
      static constexpr float hug_seconds                 = 0.9F;
      static constexpr float crumble_duration_seconds    = 0.7F;
      static constexpr float petrify_recovery_per_second = 0.5F;

      void advanceCrumbling(
        GameContextDto &context,
        float           delta_seconds
      );

      void recoverFromPetrify(float delta_seconds);

      MobEntity &owner_;
      float      frenzy_seconds_  = 0;
      float      petrify_amount_  = 0;
      float      crumble_seconds_ = 0;
      float      burn_seconds_    = 0;
      bool       is_frozen_       = false;
      bool       is_being_hugged_ = false;
  };
} // namespace wheel_of_fear
