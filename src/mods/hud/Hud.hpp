#pragma once

#include <optional>
#include <string>

#include "core/input/enums/InputActionEnum.hpp"
#include "core/math/Color.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/hud/MessageFeed.hpp"
#include "mods/hud/PromptBar.hpp"

namespace wheel_of_fear {
  class Hud {
    public:
      void beginFrame(float real_delta_seconds);

      void clearMessages() {
        messages_.clear();
      }

      void announce(
        Color       color,
        std::string text
      ) {
        messages_.announce(color, std::move(text));
      }

      void showPrompt(
        std::optional<InputActionEnum> key,
        std::string                    text
      ) {
        prompt_.show(key, std::move(text));
      }

      void setVisible(bool is_visible) {
        is_visible_ = is_visible;
      }

      void draw(GameContextDto &context) const;

    private:
      MessageFeed messages_;
      PromptBar   prompt_;
      bool        is_visible_ = false;
  };
} // namespace wheel_of_fear
