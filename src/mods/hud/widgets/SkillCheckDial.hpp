#pragma once

#include "core/render/Renderer.hpp"

namespace wheel_of_fear {
  class SkillCheck;

  class SkillCheckDial final {
    public:
      SkillCheckDial() = delete;

      static void draw(
        Renderer         &renderer,
        const SkillCheck &check,
        float             real_seconds
      );

    private:
      static void drawResult(
        Renderer         &renderer,
        const SkillCheck &check,
        float             real_seconds
      );
  };
} // namespace wheel_of_fear
