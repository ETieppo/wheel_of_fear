#pragma once

#include <cstddef>

#include "core/entity/Entity.hpp"
#include "game/dtos/GameContextDto.hpp"

namespace wheel_of_fear {
  class ExitDoor final: public Entity {
    public:
      ExitDoor()
          : Entity(
              Vec2 {},
              0
            ) {
      }

      bool isUnlocked() const {
        return is_unlocked_;
      }

      float openAmount() const {
        return open_amount_;
      }

      bool hasPlayerExited() const {
        return has_player_exited_;
      }

      void reset(
        Vec2 doorway_position
      );
      void unlock(
        GameContextDto &context
      );
      void update(
        GameContextDto &context,
        std::size_t     online_count,
        std::size_t     generator_count
      );
      void draw(
        GameContextDto &context
      ) const;

    private:
      Vec2 entryPoint() const {
        return position().add(Vec2::of(0, 6));
      }

      bool  is_unlocked_       = false;
      float open_amount_       = 0;
      bool  has_player_exited_ = false;
  };
} // namespace wheel_of_fear
