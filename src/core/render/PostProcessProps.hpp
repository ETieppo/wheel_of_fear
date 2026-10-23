#pragma once

#include <array>

#include "core/render/GpuLightProps.hpp"
#include "core/render/RenderConfig.hpp"

namespace wheel_of_fear {
  struct PostProcessProps {
      std::array<float, 2>                                    view_origin {};
      std::array<float, 2>                                    view_size {};
      std::array<float, 2>                                    screen_size {};
      float                                                   time        = 0;
      float                                                   light_count = 0;
      std::array<float, 4>                                    ambient {};
      std::array<float, 4>                                    effects {};
      std::array<float, 4>                                    tint {};
      std::array<float, 4>                                    map_info {};
      std::array<float, 4>                                    target_info {};
      std::array<float, 4>                                    viewer {};
      std::array<GpuLightProps, RenderConfig::max_gpu_lights> lights {};
  };

  static_assert(sizeof(PostProcessProps) == 2176);
} // namespace wheel_of_fear
