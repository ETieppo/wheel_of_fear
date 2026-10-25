#pragma once

#include "core/entity/Entity.hpp"
#include "game/dtos/GameContextDto.hpp"
#include <cstddef>

namespace wheel_of_fear {
  class TileMapModel;

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

      void reset(
        TileMapModel &map,
        Vec2Props     doorway_position
      );

      void unlock(GameContextDto &context);

      void update(
        GameContextDto &context,
        std::size_t     online_count,
        std::size_t     generator_count
      );

      void draw(GameContextDto &context) const;

    private:
      static constexpr int   footprint_half_width = 1;
      static constexpr float passable_open_amount = 0.6F;

      [[nodiscard]]
      Vec2Props entryPoint() const {
        return position().add(Vec2Props::of(0, 6));
      }

      void setFootprintBlocked(
        TileMapModel &map,
        bool          is_blocked
      ) const;

      bool  is_sealed_         = false;
      bool  is_unlocked_       = false;
      float open_amount_       = 0;
      bool  has_player_exited_ = false;
  };
} // namespace wheel_of_fear
