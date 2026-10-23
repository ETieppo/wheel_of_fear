#pragma once

#include "core/render/gpu/GpuHandle.hpp"

namespace wheel_of_fear {
  struct GpuOwnedTextureProps {
      GpuTexture     texture;
      GpuTextureView view;
  };
} // namespace wheel_of_fear
