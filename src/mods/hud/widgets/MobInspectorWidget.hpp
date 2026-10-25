#pragma once

#include "core/render/Renderer.hpp"
#include <string_view>

namespace wheel_of_fear {
  class MobInspectorWidget final {
    public:
      MobInspectorWidget() = delete;

      static void draw(
        Renderer        &renderer,
        std::string_view mob_name
      );

    private:
      static constexpr float panel_top     = 14;
      static constexpr float panel_height  = 30;
      static constexpr float text_size     = 16;
      static constexpr float panel_padding = 18;
      static constexpr float minimum_width = 120;
  };
} // namespace wheel_of_fear
