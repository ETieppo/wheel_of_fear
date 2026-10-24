#pragma once

#include "core/math/Vec2Props.hpp"
#include "core/render/RenderConfig.hpp"
#include "mods/world/TileCoordinateProps.hpp"
#include <cstddef>

namespace wheel_of_fear {
  struct DoorwayProps {
      static constexpr int length = 3;

      int         first_x              = 0;
      int         first_y              = 0;
      bool        is_vertical          = false;
      std::size_t room_a               = 0;
      std::size_t room_b               = 0;
      bool        is_open              = false;
      float       open_amount          = 0;
      float       seconds_since_change = 100;

      [[nodiscard]]
      TileCoordinateProps tileAt(int step) const {
        return is_vertical
          ? TileCoordinateProps {.x = first_x, .y = first_y + step}
          : TileCoordinateProps {.x = first_x + step, .y = first_y};
      }

      [[nodiscard]]
      Vec2Props center() const {
        const float half_span = static_cast<float>(length) / 2;
        const auto  x         = static_cast<float>(first_x);
        const auto  y         = static_cast<float>(first_y);

        if(is_vertical)
          return Vec2Props::of(
            (x + 0.5F) * RenderConfig::tile_size,
            (y + half_span) * RenderConfig::tile_size
          );

        return Vec2Props::of(
          (x + half_span) * RenderConfig::tile_size,
          (y + 0.5F) * RenderConfig::tile_size
        );
      }

      [[nodiscard]]
      bool connects(std::size_t room_index) const {
        return room_a == room_index || room_b == room_index;
      }

      [[nodiscard]]
      std::size_t otherRoom(std::size_t room_index) const {
        return room_a == room_index ? room_b : room_a;
      }
  };
} // namespace wheel_of_fear
