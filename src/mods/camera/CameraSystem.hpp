#pragma once

#include "core/input/InputController.hpp"
#include "core/math/Vec2Props.hpp"
#include "game/dtos/GameContextDto.hpp"
#include <cstddef>
#include <optional>

namespace wheel_of_fear {
  class WorldSystem;
  class PlayerEntity;

  class CameraSystem {
    public:
      [[nodiscard]]
      Vec2Props focus() const {
        return focus_;
      }

      void setFocus(Vec2Props focus) {
        focus_ = focus;
      }

      [[nodiscard]]
      std::optional<std::size_t> roomIndex() const {
        return room_index_;
      }

      void stopFollowing() {
        is_following_player_ = false;
      }

      [[nodiscard]]
      Vec2Props viewCenter() const {
        return focus_.add(shake_offset_);
      }

      [[nodiscard]]
      Vec2Props viewOrigin() const;

      [[nodiscard]]
      Vec2Props mouseWorldPosition(const InputController &input) const;

      void snapToPlayer(
        const WorldSystem  &world,
        const PlayerEntity &player
      );

      void glideTo(
        Vec2Props point,
        float     delta_seconds
      );

      void update(GameContextDto &context);

    private:
      static constexpr float shake_amplitude = 9;

      Vec2Props                  focus_ {};
      Vec2Props                  shake_offset_ {};
      std::optional<std::size_t> room_index_;
      bool                       is_following_player_ = false;
  };
} // namespace wheel_of_fear
