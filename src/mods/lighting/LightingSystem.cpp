#include "mods/lighting/LightingSystem.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/RenderConfig.hpp"
#include "mods/world/TileMapModel.hpp"
#include <algorithm>
#include <numeric>

namespace wheel_of_fear {
  void LightingSystem::add(const LightProps &light) {
    if(lights_.size() >= capacity) return;
    lights_.push_back(light);
  }

  float LightingSystem::freezingLightAt(
    const TileMapModel &map,
    Vec2Props           point,
    LightFilterEnum     filter
  ) const {
    return sumLight(map, point, filter, true);
  }

  float LightingSystem::visibilityAt(
    const TileMapModel &map,
    Vec2Props           point,
    LightFilterEnum     filter
  ) const {
    return sumLight(map, point, filter, false);
  }

  float LightingSystem::sumLight(
    const TileMapModel &map,
    Vec2Props           point,
    LightFilterEnum     filter,
    bool                only_freezing
  ) const {
    float total = 0;

    for(const LightProps &light : lights_) {
      if(only_freezing && !light.freezes_creatures) continue;
      if(filter == LightFilterEnum::ultraviolet_only && !light.is_ultraviolet)
        continue;

      const Vec2Props offset   = point.sub(light.position);
      const float     distance = offset.length();
      if(distance > light.radius) continue;

      float strength = 1.0F - (distance / light.radius);
      strength *= strength;

      if(light.isSpotlight()) {
        const float alignment = offset.normalized().dot(light.direction);
        const float cone      = ScalarMath::smoothstep(
          light.cone_outer_cosine,
          light.cone_inner_cosine,
          alignment
        );

        const float halo = only_freezing
          ? 0.0F
          : ScalarMath::clamp(1 - (distance / 10), 0, 1) * 0.6F;

        strength *= std::max(cone, halo);
      }

      if(strength < 0.01F) continue;
      if(light.casts_shadows && !map.hasLineOfSight(light.position, point))
        continue;

      total += strength * light.intensity;
    }

    return total;
  }

  void LightingSystem::submitToRenderer(Renderer &renderer) const {
    const Vec2Props view_center = renderer.viewOrigin().add(
      Vec2Props::of(
        static_cast<float>(RenderConfig::view_width) / 2,
        static_cast<float>(RenderConfig::view_height) / 2
      )
    );

    std::vector<std::size_t> visible_indices;
    std::vector<float>       priorities;

    for(std::size_t index = 0; index < lights_.size(); ++index) {
      const LightProps &light = lights_[index];
      if(!renderer.isVisible(light.position, light.radius)) continue;

      visible_indices.push_back(index);
      const float spotlight_bonus
        = light.isSpotlight() ? spotlight_priority_bonus : 0.0F;

      priorities.push_back(
        light.position.distance(view_center)
        - (light.radius * 0.5F)
        - spotlight_bonus
      );
    }

    std::vector<std::size_t> order(visible_indices.size());
    std::ranges::iota(order, std::size_t {0});
    std::ranges::stable_sort(order, [&](std::size_t first, std::size_t second) {
      return priorities[first] < priorities[second];
    });

    const std::size_t submitted
      = std::min(order.size(), RenderConfig::max_gpu_lights);

    for(std::size_t slot = 0; slot < submitted; ++slot) {
      const LightProps &light = lights_[visible_indices[order[slot]]];

      renderer.addLight(
        GpuLightProps {
          .position_radius =
            {light.position.x, light.position.y, light.radius, light.intensity},
          .direction_cone =
            {
                              light.direction.x,
                              light.direction.y,
                              light.cone_outer_cosine,
                              light.cone_inner_cosine,
                              },
          .color =
            {
                              light.color.r,
                              light.color.g,
                              light.color.b,
                              light.casts_shadows ? 1.0F : 0.0F,
                              },
          .animation = {light.flicker, static_cast<float>(slot), 0, 0},
      }
      );
    }
  }
} // namespace wheel_of_fear
