#include "mods/hud/PromptBarWidget.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/UiPainter.hpp"
#include "core/theme/Palette.hpp"
#include "mods/hud/KeyLabelCatalog.hpp"
#include <algorithm>

namespace wheel_of_fear {
  void PromptBarWidget::clear() {
    text_.clear();
    key_.reset();
  }

  void PromptBarWidget::show(
    std::optional<InputActionEnum> key,
    std::string                    text
  ) {
    text_ = std::move(text);
    key_  = key;
  }

  void PromptBarWidget::draw(Renderer &renderer) const {
    if(text_.empty()) return;
    constexpr float text_size = 16;

    const std::string_view key_label
      = key_ ? KeyLabelCatalog::labelFor(*key_) : std::string_view {};
    const float key_width = key_
      ? std::max(renderer.measureText(key_label, 14) + 12.6F, 22.4F) + 10
      : 0.0F;
    const float width = renderer.measureText(text_, text_size) + key_width + 34;
    const float left  = (RenderConfig::ui_width / 2) - (width / 2);
    constexpr float top = RenderConfig::ui_height - 82;

    UiPainter::drawPanel(
      renderer,
      left,
      top,
      width,
      34,
      Palette::panel_edge,
      0.95F
    );
    float cursor = left + 16;

    if(key_)
      cursor += UiPainter::drawKeycap(
                  renderer,
                  key_label,
                  cursor,
                  top + 7,
                  14,
                  Palette::neon_cyan
                )
        + 10;

    renderer.drawText(
      text_,
      cursor,
      top + 7,
      text_size,
      Palette::text,
      TextAlignEnum::left
    );
  }
} // namespace wheel_of_fear
