#pragma once

#include <string_view>

#include "core/render/gpu/GpuHandle.hpp"

namespace wheel_of_fear {

class ShaderModuleLoader final {
public:
    ShaderModuleLoader() = delete;

    static GpuShaderModule loadWgsl(WGPUDevice device, std::string_view file_name);
};

}
