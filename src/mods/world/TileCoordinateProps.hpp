#pragma once

namespace wheel_of_fear {
  struct TileCoordinateProps {
      int x = 0;
      int y = 0;

      bool operator ==(const TileCoordinateProps &) const = default;
  };
} // namespace wheel_of_fear
