#pragma once

#include <cstdint>
#include <string_view>

#include <SDL3/SDL_video.h>

#include "core/math/Vec2.hpp"

namespace wheel_of_fear {
  class Window {
    public:
      Window(
        std::string_view title,
        int              width,
        int              height
      );
      ~Window();
      Window(
        const Window &
      ) = delete;
      Window &operator =(
        const Window &
      ) = delete;

      SDL_Window* handle() const {
        return window_;
      }

      void*         metalLayer() const;
      Vec2          logicalSize() const;
      std::uint32_t pixelWidth() const;
      std::uint32_t pixelHeight() const;

    private:
      SDL_Window* window_     = nullptr;
      void*       metal_view_ = nullptr;
  };
} // namespace wheel_of_fear
