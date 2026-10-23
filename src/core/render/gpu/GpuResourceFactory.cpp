#include "core/render/gpu/GpuResourceFactory.hpp"

#include <algorithm>
#include <array>
#include <vector>

namespace wheel_of_fear {
  namespace {
    WGPUStringView viewOf(std::string_view text) {
      return WGPUStringView {text.data(), text.size()};
    }
  } // namespace

  GpuBuffer GpuResourceFactory::createUniformBuffer(
    std::string_view label,
    std::size_t      size
  ) const {
    WGPUBufferDescriptor descriptor = WGPU_BUFFER_DESCRIPTOR_INIT;
    descriptor.label                = viewOf(label);
    descriptor.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    descriptor.size  = size;
    return GpuBuffer(wgpuDeviceCreateBuffer(device_, &descriptor));
  }

  GpuBuffer GpuResourceFactory::createStorageBuffer(
    std::string_view label,
    std::size_t      size
  ) const {
    WGPUBufferDescriptor descriptor = WGPU_BUFFER_DESCRIPTOR_INIT;
    descriptor.label                = viewOf(label);
    descriptor.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage;
    descriptor.size  = size;
    return GpuBuffer(wgpuDeviceCreateBuffer(device_, &descriptor));
  }

  GpuOwnedTextureProps GpuResourceFactory::createRenderTarget(
    std::string_view  label,
    std::uint32_t     width,
    std::uint32_t     height,
    WGPUTextureFormat format
  ) const {
    WGPUTextureDescriptor descriptor = WGPU_TEXTURE_DESCRIPTOR_INIT;
    descriptor.label                 = viewOf(label);
    descriptor.usage =
      WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment;
    descriptor.size =
      WGPUExtent3D {std::max(width, 1U), std::max(height, 1U), 1};
    descriptor.format = format;
    GpuOwnedTextureProps target;
    target.texture.reset(wgpuDeviceCreateTexture(device_, &descriptor));
    target.view.reset(wgpuTextureCreateView(target.texture.get(), nullptr));
    return target;
  }

  GpuOwnedTextureProps GpuResourceFactory::createSampledTexture(
    std::string_view label,
    std::uint32_t    width,
    std::uint32_t    height,
    std::uint32_t    mip_level_count
  ) const {
    WGPUTextureDescriptor descriptor = WGPU_TEXTURE_DESCRIPTOR_INIT;
    descriptor.label                 = viewOf(label);
    descriptor.usage =
      WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
    descriptor.size          = WGPUExtent3D {width, height, 1};
    descriptor.format        = WGPUTextureFormat_RGBA8Unorm;
    descriptor.mipLevelCount = mip_level_count;
    GpuOwnedTextureProps sampled;
    sampled.texture.reset(wgpuDeviceCreateTexture(device_, &descriptor));
    sampled.view.reset(wgpuTextureCreateView(sampled.texture.get(), nullptr));
    return sampled;
  }

  void GpuResourceFactory::writeTexture(
    WGPUTexture                   texture,
    std::uint32_t                 mip_level,
    std::uint32_t                 width,
    std::uint32_t                 height,
    std::span<const std::uint8_t> pixels
  ) const {
    WGPUTexelCopyTextureInfo destination = WGPU_TEXEL_COPY_TEXTURE_INFO_INIT;
    destination.texture                  = texture;
    destination.mipLevel                 = mip_level;
    WGPUTexelCopyBufferLayout layout     = WGPU_TEXEL_COPY_BUFFER_LAYOUT_INIT;
    layout.bytesPerRow                   = width * 4;
    layout.rowsPerImage                  = height;
    const WGPUExtent3D extent {width, height, 1};
    wgpuQueueWriteTexture(
      queue_, &destination, pixels.data(), pixels.size(), &layout, &extent
    );
  }

  std::vector<std::uint8_t> GpuResourceFactory::downsample(
    std::span<const std::uint8_t> pixels,
    std::uint32_t                 width,
    std::uint32_t                 height
  ) {
    const std::uint32_t       next_width  = width / 2;
    const std::uint32_t       next_height = height / 2;
    std::vector<std::uint8_t> result(
      static_cast<std::size_t>(next_width) * next_height * 4
    );
    constexpr std::array<std::array<std::uint32_t, 2>, 4> offsets {
      {{0, 0}, {1, 0}, {0, 1}, {1, 1}}
    };
    for(std::uint32_t y = 0; y < next_height; ++y) {
      for(std::uint32_t x = 0; x < next_width; ++x) {
        for(std::uint32_t channel = 0; channel < 4; ++channel) {
          std::uint32_t sum = 0;
          for(const auto &offset : offsets) {
            sum += pixels
              [(static_cast<std::size_t>(y * 2 + offset[1]) *
                width +
                x *
                2 +
                offset[0]) *
               4 +
               channel];
          }
          result[(static_cast<std::size_t>(y) * next_width + x) * 4 + channel] =
            static_cast<std::uint8_t>(sum / 4);
        }
      }
    }
    return result;
  }

  GpuOwnedTextureProps GpuResourceFactory::uploadMipmappedTexture(
    std::string_view         label,
    const DecodedImageProps &image
  ) const {
    std::uint32_t level_count = 1;
    while((image.width >> level_count) >= 4) level_count += 1;
    GpuOwnedTextureProps uploaded =
      createSampledTexture(label, image.width, image.height, level_count);
    std::uint32_t             width        = image.width;
    std::uint32_t             height       = image.height;
    std::vector<std::uint8_t> level_pixels = image.pixels;
    for(std::uint32_t level = 0; level < level_count; ++level) {
      writeTexture(uploaded.texture.get(), level, width, height, level_pixels);
      if(level + 1 == level_count) break;
      level_pixels = downsample(level_pixels, width, height);
      width /= 2;
      height /= 2;
    }
    return uploaded;
  }

  GpuSampler GpuResourceFactory::createSampler(
    std::string_view label,
    WGPUAddressMode  address_mode,
    std::uint16_t    max_anisotropy
  ) const {
    WGPUSamplerDescriptor descriptor = WGPU_SAMPLER_DESCRIPTOR_INIT;
    descriptor.label                 = viewOf(label);
    descriptor.addressModeU          = address_mode;
    descriptor.addressModeV          = address_mode;
    descriptor.magFilter             = WGPUFilterMode_Linear;
    descriptor.minFilter             = WGPUFilterMode_Linear;
    descriptor.mipmapFilter          = WGPUMipmapFilterMode_Linear;
    descriptor.maxAnisotropy         = max_anisotropy;
    return GpuSampler(wgpuDeviceCreateSampler(device_, &descriptor));
  }

  GpuBindGroupLayout GpuResourceFactory::createBindGroupLayout(
    std::string_view                          label,
    std::span<const WGPUBindGroupLayoutEntry> entries
  ) const {
    WGPUBindGroupLayoutDescriptor descriptor =
      WGPU_BIND_GROUP_LAYOUT_DESCRIPTOR_INIT;
    descriptor.label      = viewOf(label);
    descriptor.entryCount = entries.size();
    descriptor.entries    = entries.data();
    return GpuBindGroupLayout(
      wgpuDeviceCreateBindGroupLayout(device_, &descriptor)
    );
  }

  GpuBindGroup GpuResourceFactory::createBindGroup(
    std::string_view                    label,
    WGPUBindGroupLayout                 layout,
    std::span<const WGPUBindGroupEntry> entries
  ) const {
    WGPUBindGroupDescriptor descriptor = WGPU_BIND_GROUP_DESCRIPTOR_INIT;
    descriptor.label                   = viewOf(label);
    descriptor.layout                  = layout;
    descriptor.entryCount              = entries.size();
    descriptor.entries                 = entries.data();
    return GpuBindGroup(wgpuDeviceCreateBindGroup(device_, &descriptor));
  }

  GpuRenderPipeline GpuResourceFactory::createPipeline(
    WGPUBindGroupLayout                   layout,
    WGPUShaderModule                      shader,
    std::span<const WGPUColorTargetState> targets
  ) const {
    WGPUPipelineLayoutDescriptor layout_descriptor =
      WGPU_PIPELINE_LAYOUT_DESCRIPTOR_INIT;
    layout_descriptor.label                = viewOf("pipeline layout");
    layout_descriptor.bindGroupLayoutCount = 1;
    layout_descriptor.bindGroupLayouts     = &layout;
    const GpuPipelineLayout pipeline_layout(
      wgpuDeviceCreatePipelineLayout(device_, &layout_descriptor)
    );

    WGPUFragmentState fragment = WGPU_FRAGMENT_STATE_INIT;
    fragment.module            = shader;
    fragment.entryPoint        = viewOf("fs");
    fragment.targetCount       = targets.size();
    fragment.targets           = targets.data();

    WGPURenderPipelineDescriptor descriptor =
      WGPU_RENDER_PIPELINE_DESCRIPTOR_INIT;
    descriptor.label              = viewOf("pipeline");
    descriptor.layout             = pipeline_layout.get();
    descriptor.vertex.module      = shader;
    descriptor.vertex.entryPoint  = viewOf("vs");
    descriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    descriptor.primitive.cullMode = WGPUCullMode_None;
    descriptor.fragment           = &fragment;
    return GpuRenderPipeline(
      wgpuDeviceCreateRenderPipeline(device_, &descriptor)
    );
  }

  WGPUBindGroupLayoutEntry
  GpuResourceFactory::uniformEntry(std::uint32_t binding) {
    WGPUBindGroupLayoutEntry entry = WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT;
    entry.binding                  = binding;
    entry.visibility  = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
    entry.buffer.type = WGPUBufferBindingType_Uniform;
    return entry;
  }

  WGPUBindGroupLayoutEntry
  GpuResourceFactory::storageEntry(std::uint32_t binding) {
    WGPUBindGroupLayoutEntry entry = WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT;
    entry.binding                  = binding;
    entry.visibility               = WGPUShaderStage_Vertex;
    entry.buffer.type              = WGPUBufferBindingType_ReadOnlyStorage;
    return entry;
  }

  WGPUBindGroupLayoutEntry
  GpuResourceFactory::textureEntry(std::uint32_t binding) {
    WGPUBindGroupLayoutEntry entry = WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT;
    entry.binding                  = binding;
    entry.visibility               = WGPUShaderStage_Fragment;
    entry.texture.sampleType       = WGPUTextureSampleType_Float;
    entry.texture.viewDimension    = WGPUTextureViewDimension_2D;
    return entry;
  }

  WGPUBindGroupLayoutEntry
  GpuResourceFactory::samplerEntry(std::uint32_t binding) {
    WGPUBindGroupLayoutEntry entry = WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT;
    entry.binding                  = binding;
    entry.visibility               = WGPUShaderStage_Fragment;
    entry.sampler.type             = WGPUSamplerBindingType_Filtering;
    return entry;
  }

  WGPUBindGroupEntry GpuResourceFactory::bufferBinding(
    std::uint32_t binding,
    WGPUBuffer    buffer,
    std::uint64_t size
  ) {
    WGPUBindGroupEntry entry = WGPU_BIND_GROUP_ENTRY_INIT;
    entry.binding            = binding;
    entry.buffer             = buffer;
    entry.size               = size;
    return entry;
  }

  WGPUBindGroupEntry GpuResourceFactory::textureBinding(
    std::uint32_t   binding,
    WGPUTextureView view
  ) {
    WGPUBindGroupEntry entry = WGPU_BIND_GROUP_ENTRY_INIT;
    entry.binding            = binding;
    entry.textureView        = view;
    return entry;
  }

  WGPUBindGroupEntry GpuResourceFactory::samplerBinding(
    std::uint32_t binding,
    WGPUSampler   sampler
  ) {
    WGPUBindGroupEntry entry = WGPU_BIND_GROUP_ENTRY_INIT;
    entry.binding            = binding;
    entry.sampler            = sampler;
    return entry;
  }
} // namespace wheel_of_fear
