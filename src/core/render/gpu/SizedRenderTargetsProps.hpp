#pragma once

#include "core/render/gpu/GpuHandle.hpp"
#include "core/render/gpu/GpuOwnedTextureProps.hpp"
#include <cstdint>

namespace wheel_of_fear {
  struct SizedRenderTargetsProps {
      std::uint32_t        width  = 0;
      std::uint32_t        height = 0;
      GpuOwnedTextureProps scene;
      GpuOwnedTextureProps glow;
      GpuOwnedTextureProps irradiance;
      GpuOwnedTextureProps direction;
      GpuOwnedTextureProps lit;
      GpuOwnedTextureProps bloom;
      GpuBindGroup         composite_bind_group;
      GpuBindGroup         bloom_bind_group;
      GpuBindGroup         post_bind_group;
  };
} // namespace wheel_of_fear
