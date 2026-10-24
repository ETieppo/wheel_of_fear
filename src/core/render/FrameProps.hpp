#pragma once

#include "core/render/GpuLightProps.hpp"
#include "core/render/PostProcessProps.hpp"
#include "core/render/QuadBatch.hpp"
#include "core/render/RenderConfig.hpp"
#include <array>
#include <cstdint>
#include <vector>

namespace wheel_of_fear {
  struct FrameProps {
      QuadBatch world {RenderConfig::world_quad_capacity};
      QuadBatch glow {RenderConfig::glow_quad_capacity};
      QuadBatch ui {RenderConfig::ui_quad_capacity};
      std::array<GpuLightProps, RenderConfig::max_gpu_lights> lights {};
      std::size_t                                             light_count = 0;
      PostProcessProps                                        post {};
      std::vector<std::uint8_t> map_texels = std::vector<std::uint8_t>(
        static_cast<std::size_t>(RenderConfig::max_map_size)
          * RenderConfig::max_map_size
          * 4,
        0
      );
      std::uint64_t map_version  = 0;
      std::uint64_t map_revision = 0;

      void reset() {
        world.clear();
        glow.clear();
        ui.clear();
        light_count = 0;
      }
  };
} // namespace wheel_of_fear
