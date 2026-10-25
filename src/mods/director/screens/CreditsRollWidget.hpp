#pragma once

#include "core/render/Renderer.hpp"
#include "mods/credits/CreditLineProps.hpp"
#include <span>

namespace wheel_of_fear {
  class CreditsRollWidget final {
    public:
      CreditsRollWidget() = delete;

      static void draw(
        Renderer                        &renderer,
        std::span<const CreditLineProps> lines,
        float                            scroll,
        float                            backdrop_alpha
      );
  };
} // namespace wheel_of_fear
