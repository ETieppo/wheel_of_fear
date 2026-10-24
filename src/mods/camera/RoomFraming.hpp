#pragma once

#include "core/math/Vec2.hpp"
#include "core/render/RenderConfig.hpp"
#include "mods/world/RoomProps.hpp"

namespace wheel_of_fear {
  class RoomFraming final {
    public:
      RoomFraming() = delete;

      static constexpr Vec2 half_view = Vec2 {
        static_cast<float>(RenderConfig::view_width) / 2,
        static_cast<float>(RenderConfig::view_height) / 2
      };
      static constexpr float wall_margin = RenderConfig::tile_size;

      static Vec2 clampToRoom(
        const RoomProps &room,
        Vec2             desired
      );

    private:
      static float clampAxis(
        float value,
        float minimum,
        float maximum,
        float half_extent
      );
  };
} // namespace wheel_of_fear
