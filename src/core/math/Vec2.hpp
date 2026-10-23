#pragma once

#include <cmath>

namespace wheel_of_fear {

struct Vec2 {
    float x = 0;
    float y = 0;

    static constexpr Vec2 of(float x_value, float y_value) { return Vec2{x_value, y_value}; }

    static Vec2 fromAngle(float radians) { return Vec2{std::cos(radians), std::sin(radians)}; }

    constexpr Vec2 add(Vec2 other) const { return Vec2{x + other.x, y + other.y}; }

    constexpr Vec2 sub(Vec2 other) const { return Vec2{x - other.x, y - other.y}; }

    constexpr Vec2 scale(float factor) const { return Vec2{x * factor, y * factor}; }

    constexpr float dot(Vec2 other) const { return x * other.x + y * other.y; }

    float length() const { return std::sqrt(x * x + y * y); }

    float distance(Vec2 other) const { return sub(other).length(); }

    Vec2 normalized() const {
        const float magnitude = length();
        if (magnitude < 0.0001F) return Vec2{};
        return scale(1.0F / magnitude);
    }

    constexpr Vec2 lerp(Vec2 other, float t) const { return Vec2{x + (other.x - x) * t, y + (other.y - y) * t}; }

    float angle() const { return std::atan2(y, x); }
};

}
