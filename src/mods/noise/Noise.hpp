#pragma once

#include <optional>
#include <span>
#include <vector>

#include "mods/noise/NoiseEventProps.hpp"
#include "mods/noise/dtos/HeardNoiseDto.hpp"

namespace wheel_of_fear {

class TileMap;

class Noise {
public:
    Noise() { events_.reserve(capacity); }

    void clear() { events_.clear(); }
    void update(float delta_seconds);
    void emit(Vec2 position, float radius, NoiseSourceEnum source);
    std::span<const NoiseEventProps> recent() const { return events_; }
    std::optional<HeardNoiseDto> loudestHeardAt(const TileMap& map, Vec2 listener, float hearing_factor) const;

private:
    static constexpr std::size_t capacity = 128;
    static constexpr float lifetime_seconds = 0.35F;
    static constexpr float occluded_radius_factor = 0.7F;

    std::vector<NoiseEventProps> events_;
};

}
