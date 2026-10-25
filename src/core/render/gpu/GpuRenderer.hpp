#pragma once

#include "core/render/Renderer.hpp"
#include "core/render/gpu/GpuContext.hpp"
#include "core/render/gpu/GpuResourceFactory.hpp"
#include "core/render/gpu/PersistentGpuResourcesProps.hpp"
#include "core/render/gpu/SizedRenderTargetsProps.hpp"
#include <cstddef>
#include <initializer_list>
#include <memory>

namespace wheel_of_fear {
  class GpuRenderer {
    public:
      explicit GpuRenderer(GpuContext &context);
      void render(Renderer &renderer);

      void renderInto(
        Renderer       &renderer,
        WGPUTextureView target
      );

    private:
      void createPersistentResources(Renderer &renderer);
      void uploadFrame(Renderer &renderer);

      void ensureSizedTargets(
        std::uint32_t screen_width,
        std::uint32_t screen_height
      );

      static void encodePass(
        WGPUCommandEncoder                     encoder,
        std::initializer_list<WGPUTextureView> views,
        WGPULoadOp                             load_op,
        WGPUColor                              clear_color,
        WGPURenderPipeline                     pipeline,
        WGPUBindGroup                          bind_group,
        std::size_t                            vertex_count
      );

      GpuContext                                  &context_;
      GpuResourceFactory                           factory_;
      std::unique_ptr<PersistentGpuResourcesProps> persistent_;
      std::unique_ptr<SizedRenderTargetsProps>     targets_;
  };
} // namespace wheel_of_fear
