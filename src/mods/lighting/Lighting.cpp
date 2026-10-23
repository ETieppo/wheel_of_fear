#include "mods/lighting/Lighting.hpp"

#include <algorithm>
#include <numeric>

#include "core/math/Scalar.hpp"
#include "core/render/RenderConfig.hpp"
#include "mods/world/TileMap.hpp"

namespace wheel_of_fear {
  void Lighting::add(const LightProps &light) {
    if(lights_.size() >= capacity) return;
    lights_.push_back(light);
  }

  float Lighting::freezingLightAt(
    const TileMap  &map,
    Vec2            point,
    LightFilterEnum filter
  ) const {
    return sumLight(map, point, filter, true);
  }

  float Lighting::visibilityAt(
    const TileMap  &map,
    Vec2            point,
    LightFilterEnum filter
  ) const {
    return sumLight(map, point, filter, false);
  }

  float Lighting::sumLight(
    const TileMap  &map,
    Vec2            point,
    LightFilterEnum filter,
    bool            only_freezing
  ) const {
    float total = 0;
    for(const LightProps &light : lights_) {
      if(only_freezing && !light.freezes_creatures) continue;
      if(filter == LightFilterEnum::ultraviolet_only && !light.is_ultraviolet)
        continue;
      const Vec2  offset   = point.sub(light.position);
      const float distance = offset.length();
      if(distance > light.radius) continue;
      float strength = 1.0F - distance / light.radius;
      strength *= strength;
      if(light.isSpotlight()) {
        const float alignment = offset.normalized().dot(light.direction);
        const float cone      = Scalar::smoothstep(
          light.cone_outer_cosine, light.cone_inner_cosine, alignment
        );
        const float halo =
          only_freezing ? 0.0F : Scalar::clamp(1 - distance / 10, 0, 1) * 0.6F;
        strength *= std::max(cone, halo);
      }
      if(strength < 0.01F) continue;
      if(light.casts_shadows && !map.hasLineOfSight(light.position, point))
        continue;
      total += strength * light.intensity;
    }
    return total;
  }

  void Lighting::submitToRenderer(Renderer &renderer) const {
    const Vec2 view_center = renderer.viewOrigin().add(
      Vec2::of(
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
      const float spotlight_bonus =
        light.isSpotlight() ? spotlight_priority_bonus : 0.0F;
      priorities.push_back(
        light.position.distance(view_center) -
        light.radius *
        0.5F -
        spotlight_bonus
      );
    }
    std::vector<std::size_t> order(visible_indices.size());
    std::iota(order.begin(), order.end(), std::size_t {0});
    std::stable_sort(
      order.begin(), order.end(), [&](std::size_t first, std::size_t second) {
        return priorities[first] < priorities[second];
      }
    );
    const std::size_t submitted =
      std::min(order.size(), RenderConfig::max_gpu_lights);
    for(std::size_t slot = 0; slot < submitted; ++slot) {
      const LightProps &light = lights_[visible_indices[order[slot]]];
      renderer.addLight(
        GpuLightProps {
          {light.position.x,  light.position.y,         light.radius, light.intensity},
          {light.direction.x,
           light.direction.y,
           light.cone_outer_cosine,
           light.cone_inner_cosine                                                   },
          {light.color.r,
           light.color.g,
           light.color.b,
           light.casts_shadows ? 1.0F : 0.0F                                         },
          {light.flicker,     static_cast<float>(slot), 0,            0              },
      }
      );
    }
  }
} // namespace wheel_of_fear
