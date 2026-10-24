#pragma once

#include "core/render/gpu/GpuHandle.hpp"
#include <string_view>

namespace wheel_of_fear {
  class ShaderModuleLoader final {
    public:
      ShaderModuleLoader() = delete;

      static GpuShaderModule loadWgsl(
        WGPUDevice       device,
        std::string_view file_name
      );
  };
} // namespace wheel_of_fear
