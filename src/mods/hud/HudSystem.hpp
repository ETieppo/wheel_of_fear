#pragma once

#include "core/input/enums/InputActionEnum.hpp"
#include "core/math/ColorProps.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/hud/MessageFeedWidget.hpp"
#include "mods/hud/PromptBarWidget.hpp"
#include <optional>
#include <string>

namespace wheel_of_fear {
  class HudSystem {
    public:
      void beginFrame(float real_delta_seconds);

      void clearMessages() {
        messages_.clear();
      }

      void announce(
        ColorProps  color,
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
      MessageFeedWidget messages_;
      PromptBarWidget   prompt_;
      bool              is_visible_ = false;
  };
} // namespace wheel_of_fear
