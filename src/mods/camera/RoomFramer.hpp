#pragma once

#include "core/math/Vec2Props.hpp"
#include "core/render/RenderConfig.hpp"
#include "mods/world/RoomProps.hpp"

namespace wheel_of_fear {
  class RoomFramer final {
    public:
      RoomFramer() = delete;

      static constexpr float wall_margin = RenderConfig::tile_size;

      static constexpr Vec2Props half_view = Vec2Props {
        .x = static_cast<float>(RenderConfig::view_width) / 2,
        .y = static_cast<float>(RenderConfig::view_height) / 2,
      };

      static Vec2Props clampToRoom(
        const RoomProps &room,
        Vec2Props        desired
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
