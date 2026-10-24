#pragma once

namespace wheel_of_fear {
  struct TileAreaProps {
      int left   = 0;
      int top    = 0;
      int width  = 0;
      int height = 0;

      [[nodiscard]]
      bool isEmpty() const {
        return width <= 0 || height <= 0;
      }

      [[nodiscard]]
      bool overlaps(
        const TileAreaProps &other,
        int                  margin
      ) const {
        return left
          - margin
          < other.left
          + other.width
          && other.left
          < left
          + width
          + margin
          && top
          - margin
          < other.top
          + other.height
          && other.top
          < top
          + height
          + margin;
      }
  };
} // namespace wheel_of_fear
