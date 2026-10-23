#pragma once

#include <cstddef>
#include <optional>

#include "core/input/Input.hpp"
#include "core/math/Vec2.hpp"
#include "game/dtos/GameContextDto.hpp"

namespace wheel_of_fear {
  class World;
  class Player;

  class Camera {
    public:
      Vec2 focus() const {
        return focus_;
      }

      void setFocus(Vec2 focus) {
        focus_ = focus;
      }

      std::optional<std::size_t> roomIndex() const {
        return room_index_;
      }

      float transitionSeconds() const {
        return transition_seconds_;
      }

      void stopFollowing() {
        is_following_player_ = false;
      }

      Vec2 viewCenter() const {
        return focus_.add(shake_offset_);
      }

      Vec2 viewOrigin() const;
      Vec2 mouseWorldPosition(const Input &input) const;
      void snapToPlayer(
        const World  &world,
        const Player &player
      );
      void glideTo(
        Vec2  point,
        float delta_seconds
      );
      void update(GameContextDto &context);

    private:
      static constexpr Vec2  mouse_influence {0.42F, 0.42F};
      static constexpr Vec2  maximum_mouse_offset {78, 46};
      static constexpr float follow_rate             = 6;
      static constexpr float room_transition_rate    = 3.2F;
      static constexpr float room_transition_seconds = 0.7F;
      static constexpr float shake_amplitude         = 9;

      static
      Vec2 mouseOffset(const Input &input);
      static
      Vec2 framedTarget(
        std::optional<std::size_t> room_index,
        const World               &world,
        Vec2                       anchor,
        Vec2                       offset
      );

      Vec2                       focus_ {};
      Vec2                       shake_offset_ {};
      std::optional<std::size_t> room_index_;
      float                      transition_seconds_  = 0;
      bool                       is_following_player_ = false;
  };
} // namespace wheel_of_fear
