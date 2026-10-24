#pragma once

#include "core/render/dtos/GroundMaterialsDto.hpp"
#include "core/render/gpu/GpuHandle.hpp"
#include "core/render/gpu/GpuOwnedTextureProps.hpp"
#include <array>
#include <cstdint>

namespace wheel_of_fear {
  struct PersistentGpuResourcesProps {
      GpuBindGroupLayout                                     quad_layout;
      GpuBindGroupLayout                                     light_layout;
      GpuBindGroupLayout                                     composite_layout;
      GpuBindGroupLayout                                     bloom_layout;
      GpuBindGroupLayout                                     post_layout;
      GpuRenderPipeline                                      world_pipeline;
      GpuRenderPipeline                                      ui_pipeline;
      GpuRenderPipeline                                      light_pipeline;
      GpuRenderPipeline                                      composite_pipeline;
      GpuRenderPipeline                                      bloom_pipeline;
      GpuRenderPipeline                                      post_pipeline;
      GpuOwnedTextureProps                                   atlas;
      GpuOwnedTextureProps                                   wall_map;
      std::array<GpuOwnedTextureProps, ground_texture_count> ground_materials;
      GpuSampler                                             clamp_sampler;
      GpuSampler                                             repeat_sampler;
      GpuBuffer                                              world_view_buffer;
      GpuBuffer                                              ui_view_buffer;
      GpuBuffer                                              post_buffer;
      GpuBuffer                                              world_quad_buffer;
      GpuBuffer                                              glow_quad_buffer;
      GpuBuffer                                              ui_quad_buffer;
      GpuBindGroup                                           world_bind_group;
      GpuBindGroup                                           glow_bind_group;
      GpuBindGroup                                           ui_bind_group;
      GpuBindGroup                                           light_bind_group;
      std::uint64_t uploaded_map_revision = 0;
  };
} // namespace wheel_of_fear
