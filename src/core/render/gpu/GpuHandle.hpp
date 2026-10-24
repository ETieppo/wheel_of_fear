#pragma once

#include <utility>

#include <webgpu/webgpu.h>

namespace wheel_of_fear {
  template<typename Handle, void (*Release)(Handle)> class GpuHandle {
    public:
      GpuHandle() = default;

      explicit GpuHandle(
        Handle handle
      )
          : handle_(handle) {
      }

      ~GpuHandle() {
        reset();
      }

      GpuHandle(
        const GpuHandle &
      ) = delete;
      GpuHandle &operator =(
        const GpuHandle &
      ) = delete;

      GpuHandle(
        GpuHandle &&other
      ) noexcept
          : handle_(
              std::exchange(
                other.handle_,
                nullptr
              )
            ) {
      }

      GpuHandle &operator =(
        GpuHandle &&other
      ) noexcept {
        if(this != &other) {
          reset();
          handle_ = std::exchange(other.handle_, nullptr);
        }

        return *this;
      }

      [[nodiscard]]
      Handle get() const {
        return handle_;
      }

      explicit operator bool() const {
        return handle_ != nullptr;
      }

      void reset(
        Handle handle = nullptr
      ) {
        if(handle_ != nullptr) Release(handle_);
        handle_ = handle;
      }

    private:
      Handle handle_ = nullptr;
  };

  using GpuInstance    = GpuHandle<WGPUInstance, wgpuInstanceRelease>;
  using GpuSurface     = GpuHandle<WGPUSurface, wgpuSurfaceRelease>;
  using GpuAdapter     = GpuHandle<WGPUAdapter, wgpuAdapterRelease>;
  using GpuDevice      = GpuHandle<WGPUDevice, wgpuDeviceRelease>;
  using GpuQueue       = GpuHandle<WGPUQueue, wgpuQueueRelease>;
  using GpuBuffer      = GpuHandle<WGPUBuffer, wgpuBufferRelease>;
  using GpuTexture     = GpuHandle<WGPUTexture, wgpuTextureRelease>;
  using GpuTextureView = GpuHandle<WGPUTextureView, wgpuTextureViewRelease>;
  using GpuSampler     = GpuHandle<WGPUSampler, wgpuSamplerRelease>;
  using GpuBindGroupLayout =
    GpuHandle<WGPUBindGroupLayout, wgpuBindGroupLayoutRelease>;
  using GpuBindGroup = GpuHandle<WGPUBindGroup, wgpuBindGroupRelease>;
  using GpuPipelineLayout =
    GpuHandle<WGPUPipelineLayout, wgpuPipelineLayoutRelease>;
  using GpuRenderPipeline =
    GpuHandle<WGPURenderPipeline, wgpuRenderPipelineRelease>;
  using GpuShaderModule = GpuHandle<WGPUShaderModule, wgpuShaderModuleRelease>;
  using GpuCommandEncoder =
    GpuHandle<WGPUCommandEncoder, wgpuCommandEncoderRelease>;
  using GpuCommandBuffer =
    GpuHandle<WGPUCommandBuffer, wgpuCommandBufferRelease>;
  using GpuRenderPassEncoder =
    GpuHandle<WGPURenderPassEncoder, wgpuRenderPassEncoderRelease>;
} // namespace wheel_of_fear
