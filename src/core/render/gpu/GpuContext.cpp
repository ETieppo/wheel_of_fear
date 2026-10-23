#include "core/render/gpu/GpuContext.hpp"

#include <array>
#include <stdexcept>
#include <string>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_video.h>
#include <webgpu/wgpu.h>

namespace wheel_of_fear {

namespace {

std::string toString(WGPUStringView view) {
    if (view.data == nullptr) return {};
    if (view.length == WGPU_STRLEN) return std::string(view.data);
    return std::string(view.data, view.length);
}

void reportUncapturedError(WGPUDevice const*, WGPUErrorType type, WGPUStringView message, void*, void*) {
    SDL_Log("wgpu error %d: %s", static_cast<int>(type), toString(message).c_str());
}

void reportDeviceLost(WGPUDevice const*, WGPUDeviceLostReason reason, WGPUStringView message, void*, void*) {
    if (reason == WGPUDeviceLostReason_Destroyed || reason == WGPUDeviceLostReason_CallbackCancelled) return;
    SDL_Log("wgpu device lost %d: %s", static_cast<int>(reason), toString(message).c_str());
}

struct AdapterRequestProps {
    WGPUAdapter adapter = nullptr;
    std::string message;
    bool is_done = false;
};

struct DeviceRequestProps {
    WGPUDevice device = nullptr;
    std::string message;
    bool is_done = false;
};

void receiveAdapter(WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void* userdata, void*) {
    auto* request = static_cast<AdapterRequestProps*>(userdata);
    if (status == WGPURequestAdapterStatus_Success) request->adapter = adapter;
    request->message = toString(message);
    request->is_done = true;
}

void receiveDevice(WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* userdata, void*) {
    auto* request = static_cast<DeviceRequestProps*>(userdata);
    if (status == WGPURequestDeviceStatus_Success) request->device = device;
    request->message = toString(message);
    request->is_done = true;
}

}

GpuContext::GpuContext(const Window& window) {
    WGPUInstanceDescriptor instance_descriptor = WGPU_INSTANCE_DESCRIPTOR_INIT;
    instance_.reset(wgpuCreateInstance(&instance_descriptor));
    if (!instance_) throw std::runtime_error("unable to create the WebGPU instance");
    createSurface(window);
    requestAdapter();
    requestDevice();
    queue_.reset(wgpuDeviceGetQueue(device_.get()));
    chooseSurfaceFormat();
    surface_width_ = window.pixelWidth();
    surface_height_ = window.pixelHeight();
    configureSurface();
}

GpuContext::~GpuContext() {
    if (surface_) wgpuSurfaceUnconfigure(surface_.get());
}

void GpuContext::createSurface(const Window& window) {
    WGPUSurfaceDescriptor descriptor = WGPU_SURFACE_DESCRIPTOR_INIT;
#if defined(__APPLE__)
    WGPUSurfaceSourceMetalLayer source = WGPU_SURFACE_SOURCE_METAL_LAYER_INIT;
    source.layer = window.metalLayer();
    descriptor.nextInChain = &source.chain;
#elif defined(_WIN32)
    const SDL_PropertiesID properties = SDL_GetWindowProperties(window.handle());
    WGPUSurfaceSourceWindowsHWND source = WGPU_SURFACE_SOURCE_WINDOWS_HWND_INIT;
    source.hwnd = SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
    source.hinstance = SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, nullptr);
    descriptor.nextInChain = &source.chain;
#else
    const SDL_PropertiesID properties = SDL_GetWindowProperties(window.handle());
    WGPUSurfaceSourceWaylandSurface wayland_source = WGPU_SURFACE_SOURCE_WAYLAND_SURFACE_INIT;
    WGPUSurfaceSourceXlibWindow xlib_source = WGPU_SURFACE_SOURCE_XLIB_WINDOW_INIT;
    if (void* wayland_surface = SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr)) {
        wayland_source.display = SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
        wayland_source.surface = wayland_surface;
        descriptor.nextInChain = &wayland_source.chain;
    } else {
        xlib_source.display = SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
        xlib_source.window = static_cast<std::uint64_t>(SDL_GetNumberProperty(properties, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0));
        descriptor.nextInChain = &xlib_source.chain;
    }
#endif
    surface_.reset(wgpuInstanceCreateSurface(instance_.get(), &descriptor));
    if (!surface_) throw std::runtime_error("unable to create the window surface");
}

void GpuContext::requestAdapter() {
    WGPURequestAdapterOptions options = WGPU_REQUEST_ADAPTER_OPTIONS_INIT;
    options.compatibleSurface = surface_.get();
    options.powerPreference = WGPUPowerPreference_HighPerformance;
    AdapterRequestProps request;
    WGPURequestAdapterCallbackInfo callback = WGPU_REQUEST_ADAPTER_CALLBACK_INFO_INIT;
    callback.mode = WGPUCallbackMode_AllowProcessEvents;
    callback.callback = receiveAdapter;
    callback.userdata1 = &request;
    wgpuInstanceRequestAdapter(instance_.get(), &options, callback);
    while (!request.is_done) wgpuInstanceProcessEvents(instance_.get());
    if (request.adapter == nullptr) throw std::runtime_error("no WebGPU adapter: " + request.message);
    adapter_.reset(request.adapter);
}

void GpuContext::requestDevice() {
    WGPUDeviceDescriptor descriptor = WGPU_DEVICE_DESCRIPTOR_INIT;
    descriptor.label = WGPUStringView{"wheel of fear device", WGPU_STRLEN};
    descriptor.uncapturedErrorCallbackInfo.callback = reportUncapturedError;
    descriptor.deviceLostCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;
    descriptor.deviceLostCallbackInfo.callback = reportDeviceLost;
    DeviceRequestProps request;
    WGPURequestDeviceCallbackInfo callback = WGPU_REQUEST_DEVICE_CALLBACK_INFO_INIT;
    callback.mode = WGPUCallbackMode_AllowProcessEvents;
    callback.callback = receiveDevice;
    callback.userdata1 = &request;
    wgpuAdapterRequestDevice(adapter_.get(), &descriptor, callback);
    while (!request.is_done) wgpuInstanceProcessEvents(instance_.get());
    if (request.device == nullptr) throw std::runtime_error("no WebGPU device: " + request.message);
    device_.reset(request.device);
}

void GpuContext::chooseSurfaceFormat() {
    WGPUSurfaceCapabilities capabilities = WGPU_SURFACE_CAPABILITIES_INIT;
    if (wgpuSurfaceGetCapabilities(surface_.get(), adapter_.get(), &capabilities) != WGPUStatus_Success) {
        throw std::runtime_error("unable to query the surface capabilities");
    }
    constexpr std::array<WGPUTextureFormat, 2> preferred{WGPUTextureFormat_BGRA8Unorm, WGPUTextureFormat_RGBA8Unorm};
    surface_format_ = capabilities.formatCount > 0 ? capabilities.formats[0] : WGPUTextureFormat_BGRA8Unorm;
    bool found = false;
    for (const WGPUTextureFormat candidate : preferred) {
        for (std::size_t index = 0; index < capabilities.formatCount && !found; ++index) {
            if (capabilities.formats[index] == candidate) {
                surface_format_ = candidate;
                found = true;
            }
        }
    }
    wgpuSurfaceCapabilitiesFreeMembers(capabilities);
}

void GpuContext::configureSurface() {
    WGPUSurfaceConfiguration configuration = WGPU_SURFACE_CONFIGURATION_INIT;
    configuration.device = device_.get();
    configuration.format = surface_format_;
    configuration.usage = WGPUTextureUsage_RenderAttachment;
    configuration.width = surface_width_ > 0 ? surface_width_ : 1;
    configuration.height = surface_height_ > 0 ? surface_height_ : 1;
    configuration.presentMode = WGPUPresentMode_Fifo;
    configuration.alphaMode = WGPUCompositeAlphaMode_Auto;
    wgpuSurfaceConfigure(surface_.get(), &configuration);
    needs_reconfigure_ = false;
}

void GpuContext::resize(std::uint32_t width, std::uint32_t height) {
    if (width == surface_width_ && height == surface_height_ && !needs_reconfigure_) return;
    surface_width_ = width;
    surface_height_ = height;
    configureSurface();
}

std::optional<GpuTexture> GpuContext::acquireSurfaceTexture() {
    if (needs_reconfigure_) configureSurface();
    WGPUSurfaceTexture surface_texture = WGPU_SURFACE_TEXTURE_INIT;
    wgpuSurfaceGetCurrentTexture(surface_.get(), &surface_texture);
    switch (static_cast<int>(surface_texture.status)) {
    case WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal: return GpuTexture(surface_texture.texture);
    case WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal:
        needs_reconfigure_ = true;
        return GpuTexture(surface_texture.texture);
    case WGPUSurfaceGetCurrentTextureStatus_Timeout:
    case WGPUSurfaceGetCurrentTextureStatus_Occluded: break;
    default: needs_reconfigure_ = true; break;
    }
    if (surface_texture.texture != nullptr) wgpuTextureRelease(surface_texture.texture);
    return std::nullopt;
}

void GpuContext::present() {
    wgpuSurfacePresent(surface_.get());
}

}
