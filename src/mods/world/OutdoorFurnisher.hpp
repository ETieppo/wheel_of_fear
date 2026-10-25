#pragma once

#include "mods/world/RoomProps.hpp"
#include <cstddef>

namespace wheel_of_fear {
  class WorldSystem;

  class OutdoorFurnisher {
    public:
      explicit OutdoorFurnisher(WorldSystem &world)
          : world_(world) {
      }

      void plantTrees(const RoomProps &clearing);

    private:
      static constexpr int         tree_footprint     = 2;
      static constexpr int         border_depth       = 4;
      static constexpr int         border_spacing     = 3;
      static constexpr int         scattered_attempts = 140;
      static constexpr int         path_half_width    = 5;
      static constexpr int         start_clear_radius = 7;
      static constexpr std::size_t tree_variant_count = 13;

      void plantBorder(const RoomProps &clearing);
      void plantScattered(const RoomProps &clearing);

      [[nodiscard]]
      bool isOnOpenPath(
        const RoomProps &clearing,
        int              x,
        int              y
      ) const;

      bool tryPlant(
        int x,
        int y
      );

      WorldSystem &world_;
  };
} // namespace wheel_of_fear
