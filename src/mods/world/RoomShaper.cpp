#include "mods/world/RoomShaper.hpp"
#include <algorithm>

namespace wheel_of_fear {
  void RoomShaper::shape(std::size_t room_index) {
    const RoomProps room = map_.rooms()[room_index];

    protected_zones_ = protectedZones(room_index, room);
    cutouts_.clear();
    cutCorner(room, false, false);
    cutCorner(room, true, false);
    cutCorner(room, false, true);
    cutCorner(room, true, true);

    const int notch_count = random_.intInclusive(0, maximum_edge_notches);
    for(int notch = 0; notch < notch_count; ++notch) cutEdgeNotch(room);
    for(const TileAreaProps &cutout : cutouts_) apply(cutout);
  }

  std::vector<TileAreaProps> RoomShaper::protectedZones(
    std::size_t      room_index,
    const RoomProps &room
  ) const {
    const int center_x = room.centerTileX();
    const int center_y = room.centerTileY();

    const TileAreaProps clearing {
      .left   = center_x - clearing_half_width,
      .top    = center_y - clearing_half_height,
      .width  = (clearing_half_width * 2) + 1,
      .height = (clearing_half_height * 2) + 1,
    };
    const TileAreaProps exit_corridor {
      .left   = center_x - exit_corridor_half,
      .top    = room.top,
      .width  = (exit_corridor_half * 2) + 1,
      .height = center_y - room.top + 1,
    };

    std::vector<TileAreaProps> zones {clearing, exit_corridor};

    for(const DoorwayProps &doorway : map_.doorways()) {
      if(!doorway.connects(room_index)) continue;
      if(doorway.is_vertical) {
        const bool is_room_on_right = room.left == doorway.first_x + 1;

        zones.push_back(
          TileAreaProps {
            .left   = is_room_on_right
              ? room.left
              : room.left + room.width - doorway_approach_depth,
            .top    = doorway.first_y - 1,
            .width  = doorway_approach_depth,
            .height = DoorwayProps::length + 2,
          }
        );

        continue;
      }

      const bool is_room_below = room.top == doorway.first_y + 1;
      zones.push_back(
        TileAreaProps {
          .left   = doorway.first_x - 1,
          .top    = is_room_below
            ? room.top
            : room.top + room.height - doorway_approach_depth,
          .width  = DoorwayProps::length + 2,
          .height = doorway_approach_depth,
        }
      );
    }

    return zones;
  }

  bool RoomShaper::isAllowed(const TileAreaProps &cutout) const {
    if(cutout.isEmpty()) return false;

    const bool touches_protected_zone = std::ranges::any_of(
      protected_zones_,
      [&cutout](const TileAreaProps &zone) { return cutout.overlaps(zone, 0); }
    );

    const bool touches_other_cutout
      = std::ranges::any_of(cutouts_, [&cutout](const TileAreaProps &other) {
          return cutout.overlaps(other, cutout_separation);
        });

    return !touches_protected_zone && !touches_other_cutout;
  }

  void RoomShaper::cutCorner(
    const RoomProps &room,
    bool             is_right,
    bool             is_bottom
  ) {
    if(!random_.chance(corner_cut_chance)) return;
    int width  = random_.intInclusive(minimum_cut_size, (room.width / 2) - 3);
    int height = random_.intInclusive(minimum_cut_size, (room.height / 2) - 2);

    while(width >= minimum_cut_size && height >= minimum_cut_size) {
      const TileAreaProps cutout {
        .left   = is_right ? room.left + room.width - width : room.left,
        .top    = is_bottom ? room.top + room.height - height : room.top,
        .width  = width,
        .height = height,
      };

      if(isAllowed(cutout)) {
        cutouts_.push_back(cutout);
        return;
      }

      if(width >= height) width -= 1;
      else height -= 1;
    }
  }

  void RoomShaper::cutEdgeNotch(const RoomProps &room) {
    TileAreaProps cutout;
    const int     depth       = random_.intInclusive(2, 4);
    const int     length      = random_.intInclusive(3, 7);
    const auto    edge        = random_.index(4);
    const bool    is_vertical = edge >= 2;
    const int     span        = is_vertical ? room.height : room.width;
    const int     start       = random_.intInclusive(
      minimum_cut_size,
      span - length - minimum_cut_size
    );

    switch(edge) {
      case 0:
        cutout = {
          .left   = room.left + start,
          .top    = room.top,
          .width  = length,
          .height = depth,
        };
        break;

      case 1:
        cutout = {
          .left   = room.left + start,
          .top    = room.top + room.height - depth,
          .width  = length,
          .height = depth,
        };
        break;

      case 2:
        cutout = {
          .left   = room.left,
          .top    = room.top + start,
          .width  = depth,
          .height = length,
        };
        break;

      default:
        cutout = {
          .left   = room.left + room.width - depth,
          .top    = room.top + start,
          .width  = depth,
          .height = length,
        };
        break;
    }

    if(isAllowed(cutout)) cutouts_.push_back(cutout);
  }

  void RoomShaper::apply(const TileAreaProps &cutout) {
    for(int y = cutout.top; y < cutout.top + cutout.height; ++y)
      for(int x = cutout.left; x < cutout.left + cutout.width; ++x)
        map_.sealTile(x, y);
  }
} // namespace wheel_of_fear
