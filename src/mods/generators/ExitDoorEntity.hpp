#pragma once

#include "core/entity/Entity.hpp"
#include "game/dtos/GameContextDto.hpp"
#include <cstddef>

namespace wheel_of_fear {
  class ExitDoorEntity final: public Entity {
    public:
      ExitDoorEntity()
          : Entity(
              Vec2Props::of(
                0,
                0
              ),
              0
            ) {
      }

      [[nodiscard]]
      bool isUnlocked() const {
        return is_unlocked_;
      }

      [[nodiscard]]
      float openAmount() const {
        return open_amount_;
      }

      [[nodiscard]]
      bool hasPlayerExited() const {
        return has_player_exited_;
      }

      void reset(Vec2Props doorway_position);

      void unlock(GameContextDto &context);

      void update(
        GameContextDto &context,
        std::size_t     online_count,
        std::size_t     generator_count
      );

      void draw(GameContextDto &context) const;

    private:
      [[nodiscard]]
      Vec2Props entryPoint() const {
        return position().add(Vec2Props::of(0, 6));
      }

      bool  is_unlocked_       = false;
      float open_amount_       = 0;
      bool  has_player_exited_ = false;
  };
} // namespace wheel_of_fear
