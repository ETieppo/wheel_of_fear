#pragma once

#include <array>

#include "core/math/Vec2.hpp"
#include "core/render/RenderConfig.hpp"

namespace wheel_of_fear {

struct AtlasRegionProps {
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;
    float pixels_per_unit = 1;

    Vec2 worldSize() const { return Vec2::of(width / pixels_per_unit, height / pixels_per_unit); }

    std::array<float, 4> textureCoordinates() const {
        constexpr float size = static_cast<float>(RenderConfig::atlas_size);
        return {x / size, y / size, (x + width) / size, (y + height) / size};
    }

    std::array<float, 4> mirroredTextureCoordinates() const {
        const auto coordinates = textureCoordinates();
        return {coordinates[2], coordinates[1], coordinates[0], coordinates[3]};
    }
};

}
