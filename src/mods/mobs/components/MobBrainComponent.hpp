#pragma once

#include "core/math/Vec2Props.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/mobs/MobProgressProps.hpp"
#include "mods/mobs/enums/MobIntentEnum.hpp"
#include <cstddef>
#include <optional>

namespace wheel_of_fear {
  class MobEntity;

  class MobBrainComponent {
    public:
      MobBrainComponent(
        MobEntity &owner,
        Vec2Props  position
      )
          : destination_(position)
          , owner_(owner)
          , progress_ {.last_progress_position = position} {
      }

      [[nodiscard]]
      MobIntentEnum intent() const {
        return intent_;
      }

      [[nodiscard]]
      bool isHunting() const;

      [[nodiscard]]
      float intentSeconds() const {
        return intent_seconds_;
      }

      void addIntentSeconds(float seconds) {
        intent_seconds_ += seconds;
      }

      [[nodiscard]]
      Vec2Props destination() const {
        return destination_;
      }

      void setRetreatSeconds(float seconds) {
        retreat_seconds_ = seconds;
      }

      void tickCooldowns(float delta_seconds);

      void hunt(GameContextDto &context);

      void changeIntent(
        MobIntentEnum intent,
        Vec2Props     destination
      );

      void startSearching(Vec2Props anchor);

      void strikeAndRetreat(
        GameContextDto &context,
        float           retreat_seconds
      );

      void exposeHiddenPlayer(GameContextDto &context);

    private:
      static constexpr float patrol_speed_factor      = 0.42F;
      static constexpr float investigate_speed_factor = 0.75F;
      static constexpr float search_speed_factor      = 0.38F;
      static constexpr float retreat_speed_factor     = 0.95F;
      static constexpr float arrival_distance         = 12;
      static constexpr float sight_memory_seconds     = 2.5F;
      static constexpr float search_seconds           = 7;
      static constexpr float search_radius            = 64;
      static constexpr float minimum_linger_seconds   = 1.2F;
      static constexpr float maximum_linger_seconds   = 4;
      static constexpr float strike_cooldown_seconds  = 1.1F;
      static constexpr float stuck_timeout_seconds    = 3.5F;
      static constexpr float lurk_give_up_seconds     = 6;
      static constexpr float direct_chase_distance    = 72;

      void chooseIntent(GameContextDto &context);

      void pursueIntent(GameContextDto &context);

      void trackProgress(GameContextDto &context);

      void patrol(GameContextDto &context);

      void choosePatrolRoom(GameContextDto &context);

      void investigate(GameContextDto &context);

      void search(GameContextDto &context);

      void chase(GameContextDto &context);

      void lurk(GameContextDto &context);

      void lookAround(GameContextDto &context);

      void fleeFromPlayer(GameContextDto &context);

      void strikePlayer(GameContextDto &context);

      std::optional<std::size_t> patrol_room_;
      Vec2Props                  destination_;
      MobEntity                 &owner_;
      MobIntentEnum              intent_         = MobIntentEnum::patrolling;
      float                      intent_seconds_ = 0;
      float                      linger_seconds_ = 0;
      Vec2Props                  search_anchor_ {};
      MobProgressProps           progress_;
      float                      strike_cooldown_ = 0;
      float                      retreat_seconds_ = 0;
  };
} // namespace wheel_of_fear
