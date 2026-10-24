#pragma once

#include "core/math/Vec2Props.hpp"
#include "core/render/Renderer.hpp"
#include "mods/art/SpriteSheetProps.hpp"
#include <cstddef>

namespace wheel_of_fear {
  class LockerEntity {
    public:
      static constexpr float reach = 15;

      LockerEntity(
        Vec2Props   position,
        std::size_t room_index,
        bool        faces_down
      )
          : position_(position)
          , room_index_(room_index)
          , faces_down_(faces_down) {
      }

      [[nodiscard]]
      Vec2Props position() const {
        return position_;
      }

      [[nodiscard]]
      std::size_t roomIndex() const {
        return room_index_;
      }

      [[nodiscard]]
      Vec2Props entryPoint() const;

      void swingDoor() {
        door_open_amount_ = 1;
      }

      void update(float delta_seconds);

      void draw(
        Renderer               &renderer,
        const SpriteSheetProps &sprites
      ) const;

    private:
      Vec2Props   position_;
      std::size_t room_index_;
      bool        faces_down_;
      float       door_open_amount_ = 0;
  };
} // namespace wheel_of_fear
