#pragma once

#include <vector>

#include "core/render/Renderer.hpp"
#include "mods/lighting/LightProps.hpp"
#include "mods/lighting/enums/LightFilterEnum.hpp"

namespace wheel_of_fear {
  class TileMap;

  class Lighting {
    public:
      Lighting() {
        lights_.reserve(capacity);
      }

      void beginFrame() {
        lights_.clear();
      }

      void  add(const LightProps &light);
      float freezingLightAt(
        const TileMap  &map,
        Vec2            point,
        LightFilterEnum filter
      ) const;
      float visibilityAt(
        const TileMap  &map,
        Vec2            point,
        LightFilterEnum filter
      ) const;
      void submitToRenderer(Renderer &renderer) const;

    private:
      static constexpr std::size_t capacity                 = 160;
      static constexpr float       spotlight_priority_bonus = 1000;

      float sumLight(
        const TileMap  &map,
        Vec2            point,
        LightFilterEnum filter,
        bool            only_freezing
      ) const;

      std::vector<LightProps> lights_;
  };
} // namespace wheel_of_fear
