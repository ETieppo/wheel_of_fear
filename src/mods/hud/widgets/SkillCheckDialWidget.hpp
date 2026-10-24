#pragma once

#include "core/render/Renderer.hpp"

namespace wheel_of_fear {
  class SkillCheckMinigame;

  class SkillCheckDialWidget final {
    public:
      SkillCheckDialWidget() = delete;

      static void draw(
        Renderer                 &renderer,
        const SkillCheckMinigame &check,
        float                     real_seconds
      );

    private:
      static void drawResult(
        Renderer                 &renderer,
        const SkillCheckMinigame &check,
        float                     real_seconds
      );
  };
} // namespace wheel_of_fear
