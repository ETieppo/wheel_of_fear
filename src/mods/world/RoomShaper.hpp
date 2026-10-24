#pragma once

#include "core/math/RandomGenerator.hpp"
#include "mods/world/RoomProps.hpp"
#include "mods/world/TileAreaProps.hpp"
#include "mods/world/TileMapModel.hpp"
#include <cstddef>
#include <vector>

namespace wheel_of_fear {
  class RoomShaper {
    public:
      RoomShaper(
        TileMapModel    &map,
        RandomGenerator &random
      )
          : map_(map)
          , random_(random) {
      }

      void shape(std::size_t room_index);

    private:
      static constexpr float corner_cut_chance      = 0.55F;
      static constexpr int   minimum_cut_size       = 3;
      static constexpr int   maximum_edge_notches   = 2;
      static constexpr int   clearing_half_width    = 6;
      static constexpr int   clearing_half_height   = 5;
      static constexpr int   exit_corridor_half     = 2;
      static constexpr int   doorway_approach_depth = 3;
      static constexpr int   cutout_separation      = 1;

      [[nodiscard]]
      std::vector<TileAreaProps> protectedZones(
        std::size_t      room_index,
        const RoomProps &room
      ) const;

      [[nodiscard]]
      bool isAllowed(const TileAreaProps &cutout) const;

      void cutCorner(
        const RoomProps &room,
        bool             is_right,
        bool             is_bottom
      );

      void cutEdgeNotch(const RoomProps &room);

      void apply(const TileAreaProps &cutout);

      TileMapModel              &map_;
      RandomGenerator           &random_;
      std::vector<TileAreaProps> protected_zones_;
      std::vector<TileAreaProps> cutouts_;
  };
} // namespace wheel_of_fear
