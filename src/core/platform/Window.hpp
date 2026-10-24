#pragma once

#include "core/math/Vec2Props.hpp"
#include <SDL3/SDL_video.h>
#include <cstdint>
#include <string_view>

namespace wheel_of_fear {
  class Window {
    public:
      Window(
        std::string_view title,
        int              width,
        int              height
      );
      ~Window();
      Window(const Window &)             = delete;
      Window &operator =(const Window &) = delete;

      [[nodiscard]]
      SDL_Window* handle() const {
        return window_;
      }

      [[nodiscard]]
      void* metalLayer() const;
      [[nodiscard]]
      std::uint32_t pixelWidth() const;
      [[nodiscard]]
      std::uint32_t pixelHeight() const;
      [[nodiscard]]
      Vec2Props logicalSize() const;

    private:
      SDL_Window* window_     = nullptr;
      void*       metal_view_ = nullptr;
  };
} // namespace wheel_of_fear
