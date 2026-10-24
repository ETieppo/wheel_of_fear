#pragma once

#include "core/math/Vec2Props.hpp"
#include "core/render/RenderConfig.hpp"
#include <array>

namespace wheel_of_fear {
  struct AtlasRegionProps {
      float x               = 0;
      float y               = 0;
      float width           = 0;
      float height          = 0;
      float pixels_per_unit = 1;

      [[nodiscard]]
      Vec2Props worldSize() const {
        return Vec2Props::of(width / pixels_per_unit, height / pixels_per_unit);
      }

      [[nodiscard]]
      std::array<
        float,
        4
      > textureCoordinates() const {
        constexpr auto size = static_cast<float>(RenderConfig::atlas_size);
        return {x / size, y / size, (x + width) / size, (y + height) / size};
      }

      [[nodiscard]]
      std::array<
        float,
        4
      > mirroredTextureCoordinates() const {
        const auto coordinates = textureCoordinates();
        return {coordinates[2], coordinates[1], coordinates[0], coordinates[3]};
      }
  };
} // namespace wheel_of_fear
