#pragma once

#include "core/math/Vec2Props.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/Renderer.hpp"
#include "core/render/gpu/GpuContext.hpp"
#include "core/render/gpu/GpuHandle.hpp"
#include "core/render/gpu/GpuRenderer.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <optional>
#include <vector>
#include <webgpu/wgpu.h>

namespace wheel_of_fear {
  class OffscreenFrameCapture {
    public:
      static constexpr std::uint32_t pixels_per_unit = 2;
      static constexpr std::uint32_t width
        = RenderConfig::view_width * pixels_per_unit;
      static constexpr std::uint32_t height
        = RenderConfig::view_height * pixels_per_unit;

      static std::unique_ptr<OffscreenFrameCapture> tryCreate() {
        try {
          return std::unique_ptr<OffscreenFrameCapture>(
            new OffscreenFrameCapture()
          );
        } catch(...) { return nullptr; }
      }

      void capture(Renderer &renderer) {
        view_origin_ = renderer.viewOrigin();

        gpu_renderer_->renderInto(renderer, target_view_.get());
        copyTargetToReadback();
        readPixels();
      }

      [[nodiscard]]
      std::optional<float> brightnessAt(Vec2Props world_point) const {
        const Vec2Props local = world_point.sub(view_origin_)
                                  .scale(static_cast<float>(pixels_per_unit));
        const auto      x     = static_cast<int>(local.x);
        const auto      y     = static_cast<int>(local.y);

        if(x < 0 || y < 0) return std::nullopt;
        if(x >= static_cast<int>(width) || y >= static_cast<int>(height))
          return std::nullopt;

        const std::size_t index = ((static_cast<std::size_t>(y) * width)
                                   + static_cast<std::size_t>(x))
          * 4;

        return (static_cast<float>(pixels_[index])
                + static_cast<float>(pixels_[index + 1])
                + static_cast<float>(pixels_[index + 2]))
          / (3 * 255.0F);
      }

    private:
      static constexpr std::uint32_t bytes_per_row
        = ((width * 4) + 255) / 256 * 256;

      OffscreenFrameCapture()
          : context_(
              std::make_unique<GpuContext>(
                width,
                height
              )
            )
          , gpu_renderer_(std::make_unique<GpuRenderer>(*context_)) {
        WGPUTextureDescriptor texture = WGPU_TEXTURE_DESCRIPTOR_INIT;
        texture.size
          = {.width = width, .height = height, .depthOrArrayLayers = 1};
        texture.format = WGPUTextureFormat_RGBA8Unorm;
        texture.usage
          = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_CopySrc;
        target_.reset(wgpuDeviceCreateTexture(context_->device(), &texture));
        target_view_.reset(wgpuTextureCreateView(target_.get(), nullptr));

        WGPUBufferDescriptor buffer = WGPU_BUFFER_DESCRIPTOR_INIT;
        buffer.size  = static_cast<std::uint64_t>(bytes_per_row) * height;
        buffer.usage = WGPUBufferUsage_MapRead | WGPUBufferUsage_CopyDst;
        readback_.reset(wgpuDeviceCreateBuffer(context_->device(), &buffer));
      }

      void copyTargetToReadback() {
        const GpuCommandEncoder encoder(
          wgpuDeviceCreateCommandEncoder(context_->device(), nullptr)
        );

        WGPUTexelCopyTextureInfo source = WGPU_TEXEL_COPY_TEXTURE_INFO_INIT;
        source.texture                  = target_.get();

        WGPUTexelCopyBufferInfo destination = WGPU_TEXEL_COPY_BUFFER_INFO_INIT;
        destination.buffer                  = readback_.get();
        destination.layout.bytesPerRow      = bytes_per_row;
        destination.layout.rowsPerImage     = height;

        const WGPUExtent3D size {
          .width              = width,
          .height             = height,
          .depthOrArrayLayers = 1
        };
        wgpuCommandEncoderCopyTextureToBuffer(
          encoder.get(),
          &source,
          &destination,
          &size
        );

        const GpuCommandBuffer command(
          wgpuCommandEncoderFinish(encoder.get(), nullptr)
        );
        const WGPUCommandBuffer submitted = command.get();

        wgpuQueueSubmit(context_->queue(), 1, &submitted);
      }

      void readPixels() {
        bool is_mapped = false;

        WGPUBufferMapCallbackInfo callback = WGPU_BUFFER_MAP_CALLBACK_INFO_INIT;
        callback.mode                      = WGPUCallbackMode_AllowSpontaneous;
        callback.callback
          = [](WGPUMapAsyncStatus, WGPUStringView, void* userdata, void*) {
              *static_cast<bool*>(userdata) = true;
            };
        callback.userdata1 = &is_mapped;

        wgpuBufferMapAsync(
          readback_.get(),
          WGPUMapMode_Read,
          0,
          static_cast<std::size_t>(bytes_per_row) * height,
          callback
        );

        while(!is_mapped) context_->waitForIdle();

        const auto* mapped
          = static_cast<const std::uint8_t*>(wgpuBufferGetConstMappedRange(
            readback_.get(),
            0,
            static_cast<std::size_t>(bytes_per_row) * height
          ));

        pixels_.assign(static_cast<std::size_t>(width) * height * 4, 0);

        for(std::uint32_t row = 0; row < height; ++row)
          std::memcpy(
            pixels_.data() + (static_cast<std::size_t>(row) * width * 4),
            mapped + (static_cast<std::size_t>(row) * bytes_per_row),
            static_cast<std::size_t>(width) * 4
          );

        wgpuBufferUnmap(readback_.get());
      }

      std::unique_ptr<GpuContext>  context_;
      std::unique_ptr<GpuRenderer> gpu_renderer_;
      GpuTexture                   target_;
      GpuTextureView               target_view_;
      GpuBuffer                    readback_;
      std::vector<std::uint8_t>    pixels_;
      Vec2Props                    view_origin_ {};
  };
} // namespace wheel_of_fear
