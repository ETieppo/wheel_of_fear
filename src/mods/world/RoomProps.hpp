#pragma once

#include <cstddef>
#include <cstdlib>

#include "core/math/Vec2Props.hpp"
#include "core/render/RenderConfig.hpp"
#include "mods/world/enums/RoomLayoutEnum.hpp"

namespace wheel_of_fear {
  struct RoomProps {
      static constexpr int free_border = 3;

      int            left   = 0;
      int            top    = 0;
      int            width  = 0;
      int            height = 0;
      int            column = 0;
      int            row    = 0;
      RoomLayoutEnum layout = RoomLayoutEnum::hall;

      [[nodiscard]]
      Vec2Props center() const {
        return Vec2Props::of(
          (static_cast<float>(left) + (static_cast<float>(width) / 2)) *
            RenderConfig::tile_size,
          (static_cast<float>(top) + (static_cast<float>(height) / 2)) *
            RenderConfig::tile_size
        );
      }

      [[nodiscard]]
      int centerTileX() const {
        return left + (width / 2);
      }

      [[nodiscard]]
      int centerTileY() const {
        return top + (height / 2);
      }

      [[nodiscard]]
      bool containsTile(
        int x,
        int y
      ) const {
        return x >= left && y >= top && x < left + width && y < top + height;
      }

      [[nodiscard]]
      std::size_t tileCount() const {
        return static_cast<std::size_t>(width) *
               static_cast<std::size_t>(height);
      }

      [[nodiscard]]
      std::size_t localIndex(
        int x,
        int y
      ) const {
        return (static_cast<std::size_t>(y - top) *
                static_cast<std::size_t>(width)) +
               static_cast<std::size_t>(x - left);
      }

      [[nodiscard]]
      Vec2Props worldMinimum() const {
        return Vec2Props::of(
          static_cast<float>(left) * RenderConfig::tile_size,
          static_cast<float>(top) * RenderConfig::tile_size
        );
      }

      [[nodiscard]]
      Vec2Props worldMaximum() const {
        return Vec2Props::of(
          static_cast<float>(left + width) * RenderConfig::tile_size,
          static_cast<float>(top + height) * RenderConfig::tile_size
        );
      }

      [[nodiscard]]
      int gridDistance(
        const RoomProps &other
      ) const {
        return std::abs(column - other.column) + std::abs(row - other.row);
      }
  };
} // namespace wheel_of_fear
