#include "core/render/gpu/ShaderModuleLoader.hpp"

#include <stdexcept>
#include <string>

#include "core/assets/AssetLocator.hpp"

namespace wheel_of_fear {
  GpuShaderModule ShaderModuleLoader::loadWgsl(
    WGPUDevice       device,
    std::string_view file_name
  ) {
    const std::string source =
      AssetLocator::readText(AssetLocator::shaderPath(file_name));
    const std::string    label(file_name);
    WGPUShaderSourceWGSL wgsl = WGPU_SHADER_SOURCE_WGSL_INIT;
    wgsl.code                 = WGPUStringView {source.data(), source.size()};
    WGPUShaderModuleDescriptor descriptor = WGPU_SHADER_MODULE_DESCRIPTOR_INIT;
    descriptor.nextInChain                = &wgsl.chain;
    descriptor.label = WGPUStringView {label.data(), label.size()};
    GpuShaderModule module(wgpuDeviceCreateShaderModule(device, &descriptor));
    if(!module) throw std::runtime_error("unable to compile shader " + label);
    return module;
  }
} // namespace wheel_of_fear
