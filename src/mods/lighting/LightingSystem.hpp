#pragma once

#include "core/render/Renderer.hpp"
#include "mods/lighting/LightProps.hpp"
#include "mods/lighting/enums/LightFilterEnum.hpp"
#include <vector>

namespace wheel_of_fear {
  class TileMapModel;

  class LightingSystem {
    public:
      LightingSystem() {
        lights_.reserve(capacity);
      }

      void beginFrame() {
        lights_.clear();
      }

      [[nodiscard]]
      const std::vector<LightProps> &lights() const {
        return lights_;
      }

      void add(const LightProps &light);

      [[nodiscard]]
      float freezingLightAt(
        const TileMapModel &map,
        Vec2Props           point,
        LightFilterEnum     filter
      ) const;

      [[nodiscard]]
      float visibilityAt(
        const TileMapModel &map,
        Vec2Props           point,
        LightFilterEnum     filter
      ) const;

      void submitToRenderer(Renderer &renderer) const;

    private:
      static constexpr std::size_t capacity                 = 160;
      static constexpr float       spotlight_priority_bonus = 1000;

      [[nodiscard]]
      float sumLight(
        const TileMapModel &map,
        Vec2Props           point,
        LightFilterEnum     filter,
        bool                only_freezing
      ) const;

      std::vector<LightProps> lights_;
  };
} // namespace wheel_of_fear
