#include "mods/hud/MessageFeedWidget.hpp"

#include <algorithm>

#include "core/math/ScalarMath.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/theme/Palette.hpp"

namespace wheel_of_fear {
  void MessageFeedWidget::announce(
    ColorProps  color,
    std::string text
  ) {
    if(messages_.size() == capacity) messages_.erase(messages_.begin());

    messages_.push_back(
      HudMessageProps {
        .text        = std::move(text),
        .color       = color,
        .age_seconds = 0,
      }
    );
  }

  void MessageFeedWidget::age(
    float delta_seconds
  ) {
    for(HudMessageProps &message : messages_)
      message.age_seconds += delta_seconds;

    std::erase_if(messages_, [](const HudMessageProps &message) {
      return message.age_seconds > lifetime_seconds;
    });
  }

  void MessageFeedWidget::draw(
    Renderer &renderer
  ) const {
    constexpr float line_height = 20;
    constexpr float right       = RenderConfig::ui_width - 18;

    for(std::size_t index = 0; index < messages_.size(); ++index) {
      const HudMessageProps &message = messages_[index];

      const float fade_out = ScalarMath::clamp(
        1.0F - ((message.age_seconds - (lifetime_seconds - fade_seconds)) /
                fade_seconds),
        0,
        1
      );

      const float fade_in = ScalarMath::clamp(message.age_seconds * 5, 0, 1);
      const float alpha   = fade_out * fade_in;
      const float y       = 96 + (static_cast<float>(index) * line_height);

      const std::size_t visible_length = std::min(
        message.text.size(),
        static_cast<std::size_t>(message.age_seconds * 90)
      );

      const float width = renderer.measureText(message.text, 14);

      renderer.drawUiRectangle(
        right - width - 22,
        y - 2,
        width + 26,
        18,
        Palette::panel_deep.withAlpha(0.65F * alpha)
      );

      renderer.drawUiRectangle(
        right - width - 22,
        y - 2,
        2,
        18,
        message.color.withAlpha(alpha)
      );

      renderer.drawText(
        std::string_view(message.text).substr(0, visible_length),
        right - width - 12,
        y,
        14,
        message.color.withAlpha(alpha),
        TextAlignEnum::left
      );
    }
  }
} // namespace wheel_of_fear
