#pragma once

#include "core/render/RenderConfig.hpp"
#include "mods/director/screens/MenuButtonProps.hpp"

namespace wheel_of_fear {
  class MenuButtonCatalog final {
    public:
      MenuButtonCatalog() = delete;

      static constexpr float button_width          = 140;
      static constexpr float button_height         = 32;
      static constexpr float bottom_margin         = 52;
      static constexpr float side_margin           = 24;
      static constexpr float language_button_width = 196;
      static constexpr float wide_button_width     = 176;

      static constexpr MenuButtonProps title_credits {
        .bounds = {
          .x      = RenderConfig::ui_width - side_margin - button_width,
          .y      = RenderConfig::ui_height - bottom_margin,
          .width  = button_width,
          .height = button_height,
        },
        .action = InputActionEnum::show_credits,
        .text   = {"CREDITS", "CRÉDITOS"},
      };

      static constexpr MenuButtonProps title_quit {
        .bounds = {
          .x      = side_margin,
          .y      = RenderConfig::ui_height - bottom_margin,
          .width  = button_width,
          .height = button_height,
        },
        .action = InputActionEnum::quit_game,
        .text   = {"QUIT", "SAIR"},
      };

      static constexpr MenuButtonProps title_language {
        .bounds = {
          .x      = side_margin,
          .y      = side_margin,
          .width  = language_button_width,
          .height = button_height,
        },
        .action = InputActionEnum::toggle_language,
        .text   = {"ENGLISH", "PORTUGUÊS"},
      };

      static constexpr MenuButtonProps paused_quit {
        .bounds = {
          .x      = (RenderConfig::ui_width - wide_button_width) / 2,
          .y      = 336,
          .width  = wide_button_width,
          .height = button_height,
        },
        .action = InputActionEnum::quit_game,
        .text   = {"QUIT GAME", "SAIR DO JOGO"},
      };
  };
} // namespace wheel_of_fear
