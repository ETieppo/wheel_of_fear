#pragma once

#include "core/input/InputController.hpp"
#include "core/math/Vec2Props.hpp"
#include "core/render/Renderer.hpp"
#include "mods/director/screens/MenuButtonProps.hpp"

namespace wheel_of_fear {
  class MenuButtonWidget final {
    public:
      MenuButtonWidget() = delete;

      static void draw(
        Renderer              &renderer,
        const MenuButtonProps &button,
        Vec2Props              mouse_in_ui
      );

      [[nodiscard]]
      static bool wasActivated(
        const MenuButtonProps &button,
        const InputController &input
      );
  };
} // namespace wheel_of_fear
