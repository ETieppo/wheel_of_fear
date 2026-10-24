#include "core/platform/Window.hpp"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_metal.h>
#include <SDL3/SDL_mouse.h>
#include <stdexcept>
#include <string>

namespace wheel_of_fear {
  Window::Window(
    std::string_view title,
    int              width,
    int              height
  ) {
    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS))
      throw std::runtime_error(
        std::string("SDL_Init failed: ") + SDL_GetError()
      );
    SDL_WindowFlags flags
      = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
#ifdef __APPLE__
    flags |= SDL_WINDOW_METAL;
#endif
    const std::string title_text(title);
    window_ = SDL_CreateWindow(title_text.c_str(), width, height, flags);
    if(window_ == nullptr)
      throw std::runtime_error(
        std::string("SDL_CreateWindow failed: ") + SDL_GetError()
      );
#ifdef __APPLE__
    metal_view_ = SDL_Metal_CreateView(window_);
    if(metal_view_ == nullptr)
      throw std::runtime_error(
        std::string("SDL_Metal_CreateView failed: ") + SDL_GetError()
      );
#endif
    SDL_HideCursor();
  }

  Window::~Window() {
    if(metal_view_ != nullptr) SDL_Metal_DestroyView(metal_view_);
    if(window_ != nullptr) SDL_DestroyWindow(window_);
    SDL_Quit();
  }

  void* Window::metalLayer() const {
    if(metal_view_ == nullptr) return nullptr;
    return SDL_Metal_GetLayer(metal_view_);
  }

  std::uint32_t Window::pixelWidth() const {
    int width  = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(window_, &width, &height);
    return static_cast<std::uint32_t>(width);
  }

  std::uint32_t Window::pixelHeight() const {
    int width  = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(window_, &width, &height);
    return static_cast<std::uint32_t>(height);
  }

  Vec2Props Window::logicalSize() const {
    int width  = 0;
    int height = 0;
    SDL_GetWindowSize(window_, &width, &height);
    return Vec2Props::of(static_cast<float>(width), static_cast<float>(height));
  }
} // namespace wheel_of_fear
