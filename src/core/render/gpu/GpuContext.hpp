#pragma once

#include <cstdint>
#include <optional>

#include "core/platform/Window.hpp"
#include "core/render/gpu/GpuHandle.hpp"

namespace wheel_of_fear {
  class GpuContext {
    public:
      explicit GpuContext(
        const Window &window
      );
      ~GpuContext();
      GpuContext(
        const GpuContext &
      ) = delete;
      GpuContext &operator =(
        const GpuContext &
      ) = delete;

      WGPUDevice device() const {
        return device_.get();
      }

      WGPUQueue queue() const {
        return queue_.get();
      }

      WGPUTextureFormat surfaceFormat() const {
        return surface_format_;
      }

      std::uint32_t surfaceWidth() const {
        return surface_width_;
      }

      std::uint32_t surfaceHeight() const {
        return surface_height_;
      }

      void resize(
        std::uint32_t width,
        std::uint32_t height
      );
      std::optional<GpuTexture> acquireSurfaceTexture();
      void                      present();

    private:
      void createSurface(
        const Window &window
      );
      void requestAdapter();
      void requestDevice();
      void chooseSurfaceFormat();
      void configureSurface();

      GpuInstance       instance_;
      GpuSurface        surface_;
      GpuAdapter        adapter_;
      GpuDevice         device_;
      GpuQueue          queue_;
      WGPUTextureFormat surface_format_    = WGPUTextureFormat_BGRA8Unorm;
      std::uint32_t     surface_width_     = 0;
      std::uint32_t     surface_height_    = 0;
      bool              needs_reconfigure_ = false;
  };
} // namespace wheel_of_fear
