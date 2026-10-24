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

      [[nodiscard]]
      float transitionSeconds() const {
        return transition_seconds_;
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
      static constexpr Vec2Props mouse_influence {.x = 0.42F, .y = 0.42F};
      static constexpr Vec2Props maximum_mouse_offset {.x = 78, .y = 46};
      static constexpr float     follow_rate             = 6;
      static constexpr float     room_transition_rate    = 3.2F;
      static constexpr float     room_transition_seconds = 0.7F;
      static constexpr float     shake_amplitude         = 9;

      static Vec2Props mouseOffset(const InputController &input);
      static Vec2Props framedTarget(
        std::optional<std::size_t> room_index,
        const WorldSystem         &world,
        Vec2Props                  anchor,
        Vec2Props                  offset
      );

      Vec2Props                  focus_ {};
      Vec2Props                  shake_offset_ {};
      std::optional<std::size_t> room_index_;
      float                      transition_seconds_  = 0;
      bool                       is_following_player_ = false;
  };
} // namespace wheel_of_fear
