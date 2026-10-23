#pragma once

#include <cstddef>

#include "core/math/Vec2.hpp"
#include "core/render/Renderer.hpp"
#include "mods/art/SpriteSheetProps.hpp"

namespace wheel_of_fear {

class Locker {
public:
    static constexpr float reach = 15;

    Locker(Vec2 position, std::size_t room_index, bool faces_down) : position_(position), room_index_(room_index), faces_down_(faces_down) {}

    Vec2 position() const { return position_; }
    std::size_t roomIndex() const { return room_index_; }
    Vec2 entryPoint() const;
    void swingDoor() { door_open_amount_ = 1; }
    void update(float delta_seconds);
    void draw(Renderer& renderer, const SpriteSheetProps& sprites) const;

private:
    Vec2 position_;
    std::size_t room_index_;
    bool faces_down_;
    float door_open_amount_ = 0;
};

}
