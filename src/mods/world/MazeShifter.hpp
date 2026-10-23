#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <span>
#include <vector>

#include "core/math/RandomGenerator.hpp"
#include "core/math/Vec2.hpp"
#include "mods/world/TileMap.hpp"

namespace wheel_of_fear {

class MazeShifter {
public:
    void reset();
    void animateDoorways(TileMap& map, float delta_seconds, std::span<const Vec2> occupants);
    bool advance(TileMap& map, RandomGenerator& random, std::uint32_t phase, float delta_seconds, Vec2 view_center, std::optional<std::size_t> focus_room, std::span<const Vec2> occupants);

    float timerSeconds() const { return timer_seconds_; }
    void setTimerSeconds(float seconds) { timer_seconds_ = seconds; }
    const std::array<std::optional<std::size_t>, 2>& movedDoorwayIndices() const { return moved_doorway_indices_; }

    static bool isInsideView(Vec2 point, Vec2 view_center);

private:
    static constexpr float first_shift_delay_seconds = 20;

    static std::vector<std::size_t> collectCandidates(const TileMap& map, std::size_t focus_room, Vec2 view_center, std::span<const Vec2> occupants, bool wants_open);
    static bool isOccupied(const DoorwayProps& doorway, std::span<const Vec2> occupants);

    float timer_seconds_ = first_shift_delay_seconds;
    std::array<std::optional<std::size_t>, 2> moved_doorway_indices_{};
};

}
