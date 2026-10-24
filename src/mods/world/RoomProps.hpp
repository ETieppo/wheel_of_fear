#pragma once

#include <cstdlib>

#include "core/math/Vec2.hpp"
#include "core/render/RenderConfig.hpp"
#include "mods/world/enums/RoomLayoutEnum.hpp"

namespace wheel_of_fear {
  struct RoomProps {
      int            left   = 0;
      int            top    = 0;
      int            width  = 0;
      int            height = 0;
      int            column = 0;
      int            row    = 0;
      RoomLayoutEnum layout = RoomLayoutEnum::hall;

      Vec2 center() const {
        return Vec2::of(
          (static_cast<float>(left) + static_cast<float>(width) / 2) *
            RenderConfig::tile_size,
          (static_cast<float>(top) + static_cast<float>(height) / 2) *
            RenderConfig::tile_size
        );
      }

      int centerTileX() const {
        return left + width / 2;
      }

      int centerTileY() const {
        return top + height / 2;
      }

      bool containsTile(
        int x,
        int y
      ) const {
        return x >= left && y >= top && x < left + width && y < top + height;
      }

      Vec2 worldMinimum() const {
        return Vec2::of(
          static_cast<float>(left) * RenderConfig::tile_size,
          static_cast<float>(top) * RenderConfig::tile_size
        );
      }

      Vec2 worldMaximum() const {
        return Vec2::of(
          static_cast<float>(left + width) * RenderConfig::tile_size,
          static_cast<float>(top + height) * RenderConfig::tile_size
        );
      }

      int gridDistance(
        const RoomProps &other
      ) const {
        return std::abs(column - other.column) + std::abs(row - other.row);
      }
  };
} // namespace wheel_of_fear
