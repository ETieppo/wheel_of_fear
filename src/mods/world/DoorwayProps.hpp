#pragma once

#include <cstddef>

#include "core/math/Vec2.hpp"
#include "core/render/RenderConfig.hpp"
#include "mods/world/TileCoordinateProps.hpp"

namespace wheel_of_fear {

struct DoorwayProps {
    static constexpr int length = 3;

    int first_x = 0;
    int first_y = 0;
    bool is_vertical = false;
    std::size_t room_a = 0;
    std::size_t room_b = 0;
    bool is_open = false;
    float open_amount = 0;
    float seconds_since_change = 100;

    TileCoordinateProps tileAt(int step) const {
        return is_vertical ? TileCoordinateProps{first_x, first_y + step} : TileCoordinateProps{first_x + step, first_y};
    }

    Vec2 center() const {
        const float half_span = static_cast<float>(length) / 2;
        const auto x = static_cast<float>(first_x);
        const auto y = static_cast<float>(first_y);
        if (is_vertical) return Vec2::of((x + 0.5F) * RenderConfig::tile_size, (y + half_span) * RenderConfig::tile_size);
        return Vec2::of((x + half_span) * RenderConfig::tile_size, (y + 0.5F) * RenderConfig::tile_size);
    }

    bool connects(std::size_t room_index) const { return room_a == room_index || room_b == room_index; }

    std::size_t otherRoom(std::size_t room_index) const { return room_a == room_index ? room_b : room_a; }
};

}
