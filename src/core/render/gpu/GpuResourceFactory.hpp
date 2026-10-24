#pragma once

#include "core/render/DecodedImageProps.hpp"
#include "core/render/gpu/GpuHandle.hpp"
#include "core/render/gpu/GpuOwnedTextureProps.hpp"
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <span>
#include <string_view>
#include <vector>

namespace wheel_of_fear {
  class GpuResourceFactory {
    public:
      static WGPUBindGroupLayoutEntry uniformEntry(std::uint32_t binding);
      static WGPUBindGroupLayoutEntry storageEntry(std::uint32_t binding);
      static WGPUBindGroupLayoutEntry textureEntry(std::uint32_t binding);
      static WGPUBindGroupLayoutEntry samplerEntry(std::uint32_t binding);

      GpuResourceFactory(
        WGPUDevice device,
        WGPUQueue  queue
      )
          : device_(device)
          , queue_(queue) {
      }

      [[nodiscard]]
      GpuBuffer createUniformBuffer(
        std::string_view label,
        std::size_t      size
      ) const;

      [[nodiscard]]
      GpuBuffer createStorageBuffer(
        std::string_view label,
        std::size_t      size
      ) const;

      [[nodiscard]]
      GpuOwnedTextureProps createRenderTarget(
        std::string_view  label,
        std::uint32_t     width,
        std::uint32_t     height,
        WGPUTextureFormat format
      ) const;

      [[nodiscard]]
      GpuOwnedTextureProps createSampledTexture(
        std::string_view label,
        std::uint32_t    width,
        std::uint32_t    height,
        std::uint32_t    mip_level_count
      ) const;

      [[nodiscard]]
      GpuOwnedTextureProps uploadMipmappedTexture(
        std::string_view         label,
        const DecodedImageProps &image
      ) const;

      [[nodiscard]]
      GpuSampler createSampler(
        std::string_view label,
        WGPUAddressMode  address_mode,
        std::uint16_t    max_anisotropy
      ) const;

      [[nodiscard]]
      GpuBindGroupLayout createBindGroupLayout(
        std::string_view                          label,
        std::span<const WGPUBindGroupLayoutEntry> entries
      ) const;

      GpuBindGroup createBindGroup(
        std::string_view                    label,
        WGPUBindGroupLayout                 layout,
        std::span<const WGPUBindGroupEntry> entries
      ) const;

      GpuRenderPipeline createPipeline(
        WGPUBindGroupLayout                   layout,
        WGPUShaderModule                      shader,
        std::span<const WGPUColorTargetState> targets
      ) const;

      void writeTexture(
        WGPUTexture                   texture,
        std::uint32_t                 mip_level,
        std::uint32_t                 width,
        std::uint32_t                 height,
        std::span<const std::uint8_t> pixels
      ) const;

      static WGPUBindGroupEntry bufferBinding(
        std::uint32_t binding,
        WGPUBuffer    buffer,
        std::uint64_t size
      );

      static WGPUBindGroupEntry textureBinding(
        std::uint32_t   binding,
        WGPUTextureView view
      );

      static WGPUBindGroupEntry samplerBinding(
        std::uint32_t binding,
        WGPUSampler   sampler
      );

    private:
      static std::vector<std::uint8_t> downsample(
        std::span<const std::uint8_t> pixels,
        std::uint32_t                 width,
        std::uint32_t                 height
      );

      WGPUDevice device_;
      WGPUQueue  queue_;
  };
} // namespace wheel_of_fear
