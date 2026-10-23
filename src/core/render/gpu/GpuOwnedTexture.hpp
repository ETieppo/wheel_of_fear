#pragma once

#include "core/render/gpu/GpuHandle.hpp"

namespace wheel_of_fear {
  struct GpuOwnedTexture {
      GpuTexture     texture;
      GpuTextureView view;
  };
} // namespace wheel_of_fear
