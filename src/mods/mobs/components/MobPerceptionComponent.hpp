#pragma once

#include <cstddef>

#include "core/math/Vec2Props.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/mobs/MobMemoryProps.hpp"

namespace wheel_of_fear {
  class MobEntity;

  class MobPerceptionComponent {
    public:
      explicit MobPerceptionComponent(
        MobEntity &owner
      )
          : owner_(owner) {
      }

      [[nodiscard]]
      const MobMemoryProps &memory() const {
        return memory_;
      }

      [[nodiscard]]
      bool canSeePlayerNow() const {
        return can_see_player_;
      }

      void witnessHiding(
        std::size_t locker_index
      ) {
        memory_.known_locker_index = locker_index;
      }

      void forgetLocker() {
        memory_.known_locker_index.reset();
      }

      void rememberPlayerAt(
        Vec2Props player_position
      ) {
        memory_.last_seen_player   = player_position;
        memory_.seconds_since_seen = 0;
      }

      void perceive(
        GameContextDto &context,
        bool            can_notice
      );
      void observeHiding(
        const GameContextDto &context,
        std::size_t           locker_index,
        Vec2Props             locker_position
      );
      void hearShepherd(
        Vec2Props target
      );
      [[nodiscard]]
      bool touchesPlayer(
        const GameContextDto &context,
        float                 reach
      ) const;

    private:
      static constexpr float breathing_detection_distance = 34;
      static constexpr float close_sense_distance         = 26;
      static constexpr float witness_distance             = 260;
      static constexpr float chasing_witness_distance     = 340;

      [[nodiscard]]
      bool canSeePlayer(
        const GameContextDto &context
      ) const;
      void listen(
        GameContextDto &context
      );

      MobEntity     &owner_;
      MobMemoryProps memory_ {};
      bool           can_see_player_ = false;
  };
} // namespace wheel_of_fear
