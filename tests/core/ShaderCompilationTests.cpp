#include <array>
#include <string>
#include <string_view>
#include <vector>

#include <doctest/doctest.h>
#include <webgpu/wgpu.h>

#include "core/render/gpu/GpuHandle.hpp"
#include "core/render/gpu/ShaderModuleLoader.hpp"

namespace wheel_of_fear {
  namespace {
    struct HeadlessGpuRequestProps {
        WGPUAdapter adapter = nullptr;
        WGPUDevice  device  = nullptr;
        bool        is_done = false;
    };

    std::vector<std::string> &collectedShaderErrors() {
      static std::vector<std::string> errors;
      return errors;
    }

    void collectShaderError(
      [[maybe_unused]] WGPUDevice const* device,
      [[maybe_unused]] WGPUErrorType     type,
      WGPUStringView                     message,
      [[maybe_unused]] void*             first_userdata,
      [[maybe_unused]] void*             second_userdata
    ) {
      collectedShaderErrors().emplace_back(
        message.data,
        message.length == WGPU_STRLEN ? std::string_view(message.data).size()
                                      : message.length
      );
    }

    void receiveHeadlessAdapter(
      WGPURequestAdapterStatus        status,
      WGPUAdapter                     adapter,
      [[maybe_unused]] WGPUStringView message,
      void*                           request_userdata,
      [[maybe_unused]] void*          second_userdata
    ) {
      auto* request = static_cast<HeadlessGpuRequestProps*>(request_userdata);
      if(status == WGPURequestAdapterStatus_Success) request->adapter = adapter;
      request->is_done = true;
    }

    void receiveHeadlessDevice(
      WGPURequestDeviceStatus         status,
      WGPUDevice                      device,
      [[maybe_unused]] WGPUStringView message,
      void*                           request_userdata,
      [[maybe_unused]] void*          second_userdata
    ) {
      auto* request = static_cast<HeadlessGpuRequestProps*>(request_userdata);
      if(status == WGPURequestDeviceStatus_Success) request->device = device;
      request->is_done = true;
    }

    GpuAdapter requestHeadlessAdapter(
      WGPUInstance instance
    ) {
      HeadlessGpuRequestProps   request;
      WGPURequestAdapterOptions options = WGPU_REQUEST_ADAPTER_OPTIONS_INIT;

      WGPURequestAdapterCallbackInfo callback =
        WGPU_REQUEST_ADAPTER_CALLBACK_INFO_INIT;

      callback.mode      = WGPUCallbackMode_AllowProcessEvents;
      callback.callback  = receiveHeadlessAdapter;
      callback.userdata1 = &request;
      wgpuInstanceRequestAdapter(instance, &options, callback);
      while(!request.is_done) wgpuInstanceProcessEvents(instance);

      return GpuAdapter(request.adapter);
    }

    GpuDevice requestHeadlessDevice(
      WGPUInstance instance,
      WGPUAdapter  adapter
    ) {
      HeadlessGpuRequestProps request;
      WGPUDeviceDescriptor    descriptor = WGPU_DEVICE_DESCRIPTOR_INIT;

      descriptor.uncapturedErrorCallbackInfo.callback = collectShaderError;

      WGPURequestDeviceCallbackInfo callback =
        WGPU_REQUEST_DEVICE_CALLBACK_INFO_INIT;

      callback.mode      = WGPUCallbackMode_AllowProcessEvents;
      callback.callback  = receiveHeadlessDevice;
      callback.userdata1 = &request;
      wgpuAdapterRequestDevice(adapter, &descriptor, callback);
      while(!request.is_done) wgpuInstanceProcessEvents(instance);

      return GpuDevice(request.device);
    }
  } // namespace

  TEST_CASE(
    "every WGSL shader compiles on a headless GPU device"
  ) {
    WGPUInstanceDescriptor instance_descriptor = WGPU_INSTANCE_DESCRIPTOR_INIT;
    const GpuInstance      instance(wgpuCreateInstance(&instance_descriptor));

    REQUIRE(static_cast<bool>(instance));

    const GpuAdapter adapter = requestHeadlessAdapter(instance.get());

    REQUIRE_MESSAGE(static_cast<bool>(adapter), "no headless GPU adapter");

    const GpuDevice device =
      requestHeadlessDevice(instance.get(), adapter.get());

    REQUIRE(static_cast<bool>(device));

    constexpr std::array<std::string_view, 5> shader_files {
      "quad.wgsl",
      "light.wgsl",
      "bloom.wgsl",
      "composite.wgsl",
      "post.wgsl",
    };

    for(const std::string_view shader_file : shader_files) {
      CAPTURE(shader_file);
      collectedShaderErrors().clear();

      const GpuShaderModule module =
        ShaderModuleLoader::loadWgsl(device.get(), shader_file);

      wgpuInstanceProcessEvents(instance.get());
      CHECK(static_cast<bool>(module));
      CHECK(collectedShaderErrors().empty());
      for(const std::string &error : collectedShaderErrors()) MESSAGE(error);
    }
  }
} // namespace wheel_of_fear
