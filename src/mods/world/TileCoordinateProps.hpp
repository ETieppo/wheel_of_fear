#pragma once

#include <array>

namespace wheel_of_fear {
  struct TileCoordinateProps {
      int x = 0;
      int y = 0;

      bool operator ==(const TileCoordinateProps &) const = default;
  };

  inline constexpr std::array<TileCoordinateProps, 4> four_neighbor_steps {
    {{.x = 1, .y = 0}, {.x = -1, .y = 0}, {.x = 0, .y = 1}, {.x = 0, .y = -1}},
  };
} // namespace wheel_of_fear
