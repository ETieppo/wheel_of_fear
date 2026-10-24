#include "core/render/gpu/GpuRenderer.hpp"

#include <algorithm>
#include <array>
#include <vector>

#include "core/render/QuadViewProps.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/gpu/ShaderModuleLoader.hpp"

namespace wheel_of_fear {
  namespace {
    constexpr WGPUTextureFormat hdr_format    = WGPUTextureFormat_RGBA16Float;
    constexpr WGPUTextureFormat sprite_format = WGPUTextureFormat_RGBA8Unorm;

    constexpr WGPUBlendState premultiplied_blend {
      .color =
        WGPUBlendComponent {
                            .operation = WGPUBlendOperation_Add,
                            .srcFactor = WGPUBlendFactor_One,
                            .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
                            },
      .alpha = WGPUBlendComponent {
                            .operation = WGPUBlendOperation_Add,
                            .srcFactor = WGPUBlendFactor_One,
                            .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
                            },
    };

    WGPUColorTargetState colorTarget(
      WGPUTextureFormat     format,
      const WGPUBlendState* blend
    ) {
      WGPUColorTargetState target = WGPU_COLOR_TARGET_STATE_INIT;

      target.format    = format;
      target.blend     = blend;
      target.writeMask = WGPUColorWriteMask_All;

      return target;
    }

    DecodedImageProps placeholderImage() {
      DecodedImageProps image;

      image.width  = 4;
      image.height = 4;
      image.pixels.assign(std::size_t {4} * 4 * 4, 128);

      return image;
    }
  } // namespace

  GpuRenderer::GpuRenderer(
    GpuContext &context
  )
      : context_(context)
      , factory_(
          context.device(),
          context.queue()
        ) {
  }

  void GpuRenderer::createPersistentResources(
    Renderer &renderer
  ) {
    auto resources = std::make_unique<PersistentGpuResourcesProps>();

    resources->atlas = factory_.createSampledTexture(
      "atlas",
      RenderConfig::atlas_size,
      RenderConfig::atlas_size,
      1
    );

    resources->wall_map = factory_.createSampledTexture(
      "wall map",
      RenderConfig::max_map_size,
      RenderConfig::max_map_size,
      1
    );

    resources->clamp_sampler =
      factory_.createSampler("clamp linear", WGPUAddressMode_ClampToEdge, 1);

    resources->repeat_sampler =
      factory_.createSampler("repeat linear", WGPUAddressMode_Repeat, 8);

    resources->world_view_buffer =
      factory_.createUniformBuffer("world view", sizeof(QuadViewProps));

    resources->ui_view_buffer =
      factory_.createUniformBuffer("ui view", sizeof(QuadViewProps));

    resources->post_buffer =
      factory_.createUniformBuffer("post process", sizeof(PostProcessProps));

    resources->world_quad_buffer = factory_.createStorageBuffer(
      "world quads",
      sizeof(QuadProps) * RenderConfig::world_quad_capacity
    );

    resources->glow_quad_buffer = factory_.createStorageBuffer(
      "glow quads",
      sizeof(QuadProps) * RenderConfig::glow_quad_capacity
    );

    resources->ui_quad_buffer = factory_.createStorageBuffer(
      "ui quads",
      sizeof(QuadProps) * RenderConfig::ui_quad_capacity
    );

    using Factory = GpuResourceFactory;

    const std::array quad_entries {
      Factory::uniformEntry(0),
      Factory::storageEntry(1),
      Factory::samplerEntry(2),
      Factory::textureEntry(3),
    };

    resources->quad_layout =
      factory_.createBindGroupLayout("quad layout", quad_entries);

    const std::array light_entries {
      Factory::uniformEntry(0),
      Factory::textureEntry(1),
    };

    resources->light_layout =
      factory_.createBindGroupLayout("light layout", light_entries);

    const std::array composite_entries {
      Factory::uniformEntry(0),
      Factory::textureEntry(1),
      Factory::textureEntry(2),
      Factory::textureEntry(3),
      Factory::textureEntry(4),
      Factory::textureEntry(5),
      Factory::samplerEntry(6),
      Factory::samplerEntry(7),
      Factory::textureEntry(8),
      Factory::textureEntry(9),
      Factory::textureEntry(10),
      Factory::textureEntry(11),
      Factory::textureEntry(12),
      Factory::textureEntry(13),
    };

    resources->composite_layout =
      factory_.createBindGroupLayout("composite layout", composite_entries);

    const std::array bloom_entries {
      Factory::uniformEntry(0),
      Factory::textureEntry(1),
      Factory::samplerEntry(2),
    };

    resources->bloom_layout =
      factory_.createBindGroupLayout("bloom layout", bloom_entries);

    const std::array post_entries {
      Factory::uniformEntry(0),
      Factory::textureEntry(1),
      Factory::textureEntry(2),
      Factory::samplerEntry(3),
    };

    resources->post_layout =
      factory_.createBindGroupLayout("post layout", post_entries);

    const WGPUDevice device = context_.device();

    const GpuShaderModule quad_shader =
      ShaderModuleLoader::loadWgsl(device, "quad.wgsl");

    const GpuShaderModule light_shader =
      ShaderModuleLoader::loadWgsl(device, "light.wgsl");

    const GpuShaderModule composite_shader =
      ShaderModuleLoader::loadWgsl(device, "composite.wgsl");

    const GpuShaderModule bloom_shader =
      ShaderModuleLoader::loadWgsl(device, "bloom.wgsl");

    const GpuShaderModule post_shader =
      ShaderModuleLoader::loadWgsl(device, "post.wgsl");

    const std::array world_targets {
      colorTarget(sprite_format, &premultiplied_blend),
    };

    const std::array ui_targets {
      colorTarget(context_.surfaceFormat(), &premultiplied_blend),
    };

    const std::array light_targets {
      colorTarget(hdr_format, nullptr),
      colorTarget(hdr_format, nullptr),
    };

    const std::array hdr_targets {colorTarget(hdr_format, nullptr)};

    const std::array surface_targets {
      colorTarget(context_.surfaceFormat(), nullptr),
    };

    resources->world_pipeline = factory_.createPipeline(
      resources->quad_layout.get(),
      quad_shader.get(),
      world_targets
    );

    resources->ui_pipeline = factory_.createPipeline(
      resources->quad_layout.get(),
      quad_shader.get(),
      ui_targets
    );

    resources->light_pipeline = factory_.createPipeline(
      resources->light_layout.get(),
      light_shader.get(),
      light_targets
    );

    resources->composite_pipeline = factory_.createPipeline(
      resources->composite_layout.get(),
      composite_shader.get(),
      hdr_targets
    );

    resources->bloom_pipeline = factory_.createPipeline(
      resources->bloom_layout.get(),
      bloom_shader.get(),
      hdr_targets
    );

    resources->post_pipeline = factory_.createPipeline(
      resources->post_layout.get(),
      post_shader.get(),
      surface_targets
    );

    std::optional<GroundMaterialsDto> materials =
      renderer.takeGroundMaterials();

    for(std::size_t index = 0; index < ground_texture_count; ++index) {
      if(materials) {
        resources->ground_materials[index] = factory_.uploadMipmappedTexture(
          "ground material",
          *materials->inBindingOrder()[index]
        );
      } else {
        resources->ground_materials[index] = factory_.uploadMipmappedTexture(
          "ground placeholder",
          placeholderImage()
        );
      }
    }

    const auto quad_bind_group = [&](
                                   std::string_view label,
                                   WGPUBuffer       view_buffer,
                                   WGPUBuffer       quad_buffer,
                                   std::size_t      capacity
                                 ) {
      const std::array entries {
        Factory::bufferBinding(0, view_buffer, sizeof(QuadViewProps)),
        Factory::bufferBinding(1, quad_buffer, sizeof(QuadProps) * capacity),
        Factory::samplerBinding(2, resources->clamp_sampler.get()),
        Factory::textureBinding(3, resources->atlas.view.get()),
      };

      return factory_
        .createBindGroup(label, resources->quad_layout.get(), entries);
    };

    resources->world_bind_group = quad_bind_group(
      "world quads",
      resources->world_view_buffer.get(),
      resources->world_quad_buffer.get(),
      RenderConfig::world_quad_capacity
    );

    resources->glow_bind_group = quad_bind_group(
      "glow quads",
      resources->world_view_buffer.get(),
      resources->glow_quad_buffer.get(),
      RenderConfig::glow_quad_capacity
    );

    resources->ui_bind_group = quad_bind_group(
      "ui quads",
      resources->ui_view_buffer.get(),
      resources->ui_quad_buffer.get(),
      RenderConfig::ui_quad_capacity
    );

    const std::array light_bindings {
      Factory::bufferBinding(
        0,
        resources->post_buffer.get(),
        sizeof(PostProcessProps)
      ),
      Factory::textureBinding(1, resources->wall_map.view.get()),
    };

    resources->light_bind_group = factory_.createBindGroup(
      "light bind group",
      resources->light_layout.get(),
      light_bindings
    );

    persistent_ = std::move(resources);
  }

  void GpuRenderer::ensureSizedTargets(
    std::uint32_t screen_width,
    std::uint32_t screen_height
  ) {
    const float pixels_per_unit = std::min(
      static_cast<float>(screen_width) /
        static_cast<float>(RenderConfig::view_width),
      static_cast<float>(screen_height) /
        static_cast<float>(RenderConfig::view_height)
    );

    const std::uint32_t width = std::max(
      static_cast<std::uint32_t>(
        static_cast<float>(RenderConfig::view_width) * pixels_per_unit
      ),
      RenderConfig::view_width
    );

    const std::uint32_t height = std::max(
      static_cast<std::uint32_t>(
        static_cast<float>(RenderConfig::view_height) * pixels_per_unit
      ),
      RenderConfig::view_height
    );

    if(targets_ && targets_->width == width && targets_->height == height)
      return;

    auto targets = std::make_unique<SizedRenderTargetsProps>();

    targets->width  = width;
    targets->height = height;

    targets->scene =
      factory_.createRenderTarget("scene", width, height, sprite_format);

    targets->glow =
      factory_.createRenderTarget("glow", width, height, sprite_format);

    const std::uint32_t light_width =
      width / RenderConfig::light_resolution_divisor;

    const std::uint32_t light_height =
      height / RenderConfig::light_resolution_divisor;

    targets->irradiance = factory_.createRenderTarget(
      "irradiance",
      light_width,
      light_height,
      hdr_format
    );

    targets->direction = factory_.createRenderTarget(
      "light direction",
      light_width,
      light_height,
      hdr_format
    );

    targets->lit =
      factory_.createRenderTarget("lit", width, height, hdr_format);

    targets->bloom = factory_.createRenderTarget(
      "bloom",
      width / RenderConfig::bloom_resolution_divisor,
      height / RenderConfig::bloom_resolution_divisor,
      hdr_format
    );

    using Factory = GpuResourceFactory;

    const PersistentGpuResourcesProps &resources = *persistent_;

    const WGPUBindGroupEntry post_entry = Factory::bufferBinding(
      0,
      resources.post_buffer.get(),
      sizeof(PostProcessProps)
    );

    const std::array composite_bindings {
      post_entry,
      Factory::textureBinding(1, targets->scene.view.get()),
      Factory::textureBinding(2, targets->glow.view.get()),
      Factory::textureBinding(3, targets->irradiance.view.get()),
      Factory::textureBinding(4, targets->direction.view.get()),
      Factory::textureBinding(5, resources.wall_map.view.get()),
      Factory::samplerBinding(6, resources.clamp_sampler.get()),
      Factory::samplerBinding(7, resources.repeat_sampler.get()),
      Factory::textureBinding(8, resources.ground_materials[0].view.get()),
      Factory::textureBinding(9, resources.ground_materials[1].view.get()),
      Factory::textureBinding(10, resources.ground_materials[2].view.get()),
      Factory::textureBinding(11, resources.ground_materials[3].view.get()),
      Factory::textureBinding(12, resources.ground_materials[4].view.get()),
      Factory::textureBinding(13, resources.ground_materials[5].view.get()),
    };

    targets->composite_bind_group = factory_.createBindGroup(
      "composite bind group",
      resources.composite_layout.get(),
      composite_bindings
    );

    const std::array bloom_bindings {
      post_entry,
      Factory::textureBinding(1, targets->lit.view.get()),
      Factory::samplerBinding(2, resources.clamp_sampler.get()),
    };

    targets->bloom_bind_group = factory_.createBindGroup(
      "bloom bind group",
      resources.bloom_layout.get(),
      bloom_bindings
    );

    const std::array post_bindings {
      post_entry,
      Factory::textureBinding(1, targets->lit.view.get()),
      Factory::textureBinding(2, targets->bloom.view.get()),
      Factory::samplerBinding(3, resources.clamp_sampler.get()),
    };

    targets->post_bind_group = factory_.createBindGroup(
      "post bind group",
      resources.post_layout.get(),
      post_bindings
    );

    targets_ = std::move(targets);
  }

  void GpuRenderer::uploadFrame(
    Renderer &renderer
  ) {
    PersistentGpuResourcesProps &resources = *persistent_;
    FrameProps                  &frame     = renderer.frame();
    const WGPUQueue              queue     = context_.queue();
    TextureAtlas                &atlas     = renderer.atlas();

    if(atlas.needsUpload()) {
      factory_.writeTexture(
        resources.atlas.texture.get(),
        0,
        atlas.size(),
        atlas.size(),
        atlas.pixels()
      );

      atlas.markUploaded();
    }

    if(resources.uploaded_map_version != frame.map_version) {
      resources.uploaded_map_version = frame.map_version;

      factory_.writeTexture(
        resources.wall_map.texture.get(),
        0,
        RenderConfig::max_map_size,
        RenderConfig::max_map_size,
        frame.map_texels
      );
    }

    const auto screen_width  = static_cast<float>(context_.surfaceWidth());
    const auto screen_height = static_cast<float>(context_.surfaceHeight());

    frame.post.screen_size = {screen_width, screen_height};

    const std::uint32_t light_width =
      targets_->width / RenderConfig::light_resolution_divisor;

    const std::uint32_t light_height =
      targets_->height / RenderConfig::light_resolution_divisor;

    frame.post.target_info = {
      static_cast<float>(targets_->width),
      static_cast<float>(targets_->height),
      static_cast<float>(light_width),
      static_cast<float>(light_height),
    };

    frame.post.light_count = static_cast<float>(frame.light_count);
    frame.post.lights      = frame.lights;

    constexpr auto atlas_extent = static_cast<float>(RenderConfig::atlas_size);

    const QuadViewProps world_view {
      .origin     = frame.post.view_origin,
      .size       = frame.post.view_size,
      .atlas_size = {atlas_extent, atlas_extent},
      .time       = frame.post.time,
      .padding    = 0,
    };

    const float ui_scale = std::min(
      screen_width / RenderConfig::ui_width,
      screen_height / RenderConfig::ui_height
    );

    const QuadViewProps ui_view {
      .origin =
        {
                 -(screen_width - (RenderConfig::ui_width * ui_scale)) / 2 / ui_scale,
                 -(screen_height - (RenderConfig::ui_height * ui_scale)) /
            2 /
            ui_scale, },
      .size       = {screen_width / ui_scale, screen_height / ui_scale},
      .atlas_size = {atlas_extent, atlas_extent},
      .time       = frame.post.time,
      .padding    = 0,
    };

    wgpuQueueWriteBuffer(
      queue,
      resources.world_view_buffer.get(),
      0,
      &world_view,
      sizeof(world_view)
    );

    wgpuQueueWriteBuffer(
      queue,
      resources.ui_view_buffer.get(),
      0,
      &ui_view,
      sizeof(ui_view)
    );

    wgpuQueueWriteBuffer(
      queue,
      resources.post_buffer.get(),
      0,
      &frame.post,
      sizeof(frame.post)
    );

    const auto write_quads =
      [queue](WGPUBuffer buffer, const QuadBatch &batch) {
        if(batch.count() > 0)
          wgpuQueueWriteBuffer(
            queue,
            buffer,
            0,
            batch.used().data(),
            batch.used().size_bytes()
          );
      };

    write_quads(resources.world_quad_buffer.get(), frame.world);
    write_quads(resources.glow_quad_buffer.get(), frame.glow);
    write_quads(resources.ui_quad_buffer.get(), frame.ui);
  }

  void GpuRenderer::render(
    Renderer &renderer
  ) {
    if(!persistent_) createPersistentResources(renderer);
    ensureSizedTargets(context_.surfaceWidth(), context_.surfaceHeight());
    uploadFrame(renderer);

    std::optional<GpuTexture> surface_texture =
      context_.acquireSurfaceTexture();
    if(!surface_texture) return;

    const GpuTextureView back_buffer(
      wgpuTextureCreateView(surface_texture->get(), nullptr)
    );

    const GpuCommandEncoder encoder(
      wgpuDeviceCreateCommandEncoder(context_.device(), nullptr)
    );

    const PersistentGpuResourcesProps &resources = *persistent_;
    const SizedRenderTargetsProps     &targets   = *targets_;
    const FrameProps                  &frame     = renderer.frame();
    constexpr WGPUColor opaque_black {.r = 0, .g = 0, .b = 0, .a = 1};
    constexpr WGPUColor transparent {.r = 0, .g = 0, .b = 0, .a = 0};

    encodePass(
      encoder.get(),
      {targets.scene.view.get()},
      WGPULoadOp_Clear,
      transparent,
      resources.world_pipeline.get(),
      resources.world_bind_group.get(),
      frame.world.count() * 6
    );

    encodePass(
      encoder.get(),
      {targets.glow.view.get()},
      WGPULoadOp_Clear,
      transparent,
      resources.world_pipeline.get(),
      resources.glow_bind_group.get(),
      frame.glow.count() * 6
    );

    encodePass(
      encoder.get(),
      {targets.irradiance.view.get(), targets.direction.view.get()},
      WGPULoadOp_Clear,
      transparent,
      resources.light_pipeline.get(),
      resources.light_bind_group.get(),
      3
    );

    encodePass(
      encoder.get(),
      {targets.lit.view.get()},
      WGPULoadOp_Clear,
      opaque_black,
      resources.composite_pipeline.get(),
      targets.composite_bind_group.get(),
      3
    );

    encodePass(
      encoder.get(),
      {targets.bloom.view.get()},
      WGPULoadOp_Clear,
      opaque_black,
      resources.bloom_pipeline.get(),
      targets.bloom_bind_group.get(),
      3
    );

    encodePass(
      encoder.get(),
      {back_buffer.get()},
      WGPULoadOp_Clear,
      opaque_black,
      resources.post_pipeline.get(),
      targets.post_bind_group.get(),
      3
    );

    encodePass(
      encoder.get(),
      {back_buffer.get()},
      WGPULoadOp_Load,
      transparent,
      resources.ui_pipeline.get(),
      resources.ui_bind_group.get(),
      frame.ui.count() * 6
    );

    const GpuCommandBuffer command(
      wgpuCommandEncoderFinish(encoder.get(), nullptr)
    );

    const WGPUCommandBuffer submitted = command.get();

    wgpuQueueSubmit(context_.queue(), 1, &submitted);
    context_.present();
  }

  void GpuRenderer::encodePass(
    WGPUCommandEncoder                     encoder,
    std::initializer_list<WGPUTextureView> views,
    WGPULoadOp                             load_op,
    WGPUColor                              clear_color,
    WGPURenderPipeline                     pipeline,
    WGPUBindGroup                          bind_group,
    std::size_t                            vertex_count
  ) {
    std::vector<WGPURenderPassColorAttachment> attachments;

    attachments.reserve(views.size());

    for(const WGPUTextureView view : views) {
      WGPURenderPassColorAttachment attachment =
        WGPU_RENDER_PASS_COLOR_ATTACHMENT_INIT;

      attachment.view       = view;
      attachment.loadOp     = load_op;
      attachment.storeOp    = WGPUStoreOp_Store;
      attachment.clearValue = clear_color;
      attachments.push_back(attachment);
    }

    WGPURenderPassDescriptor descriptor = WGPU_RENDER_PASS_DESCRIPTOR_INIT;

    descriptor.colorAttachmentCount = attachments.size();
    descriptor.colorAttachments     = attachments.data();

    const GpuRenderPassEncoder pass(
      wgpuCommandEncoderBeginRenderPass(encoder, &descriptor)
    );

    if(vertex_count > 0) {
      wgpuRenderPassEncoderSetPipeline(pass.get(), pipeline);
      wgpuRenderPassEncoderSetBindGroup(pass.get(), 0, bind_group, 0, nullptr);

      wgpuRenderPassEncoderDraw(
        pass.get(),
        static_cast<std::uint32_t>(vertex_count),
        1,
        0,
        0
      );
    }

    wgpuRenderPassEncoderEnd(pass.get());
  }
} // namespace wheel_of_fear
