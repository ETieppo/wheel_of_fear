#pragma once

#include <cstddef>
#include <vector>

#include "core/render/Renderer.hpp"
#include "mods/lighting/Lighting.hpp"
#include "mods/world/RoomPowerProps.hpp"
#include "mods/world/RoomProps.hpp"
#include "mods/world/TileMap.hpp"

namespace wheel_of_fear {

class RoomPowerGrid {
public:
    void clear() { rooms_.clear(); }
    void addRoom(Color color, float conduit_offset);
    void power(std::size_t room_index, bool is_powered);
    bool isPowered(std::size_t room_index) const;
    bool isFullyLit(std::size_t room_index) const;
    float litAmount(std::size_t room_index) const;
    void advance(float delta_seconds);
    void emitLights(const TileMap& map, Lighting& lighting) const;
    void draw(const TileMap& map, Renderer& renderer) const;
    Vec2 conduitPoint(const RoomProps& room, std::size_t room_index) const;

    static float perimeterOf(const RoomProps& room);

private:
    static constexpr std::size_t strip_light_count = 6;
    static constexpr float strip_light_radius = 175;
    static constexpr float strip_light_inset = 18;
    static constexpr float strip_led_spacing = 5;
    static constexpr float strip_power_up_seconds = 3.2F;
    static constexpr float strip_power_down_seconds = 1.4F;

    static Vec2 perimeterPoint(const RoomProps& room, float distance_along);
    static bool isStripLitAt(const RoomPowerProps& power, float perimeter, float distance_along);

    std::vector<RoomPowerProps> rooms_;
};

}
