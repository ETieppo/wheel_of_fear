#include "core/render/UiPainter.hpp"

#include <algorithm>
#include <numbers>

#include "core/math/ScalarMath.hpp"
#include "core/theme/Palette.hpp"

namespace wheel_of_fear {
  namespace {
    constexpr float notch = 7;
  }

  void UiPainter::drawPanel(
    Renderer  &renderer,
    float      x,
    float      y,
    float      width,
    float      height,
    ColorProps accent,
    float      alpha
  ) {
    renderer.drawUiRectangle(
      x,
      y + notch,
      width,
      height - notch,
      Palette::panel_deep.withAlpha(0.78F * alpha)
    );

    renderer.drawUiRectangle(
      x + notch,
      y,
      width - notch,
      notch,
      Palette::panel_deep.withAlpha(0.78F * alpha)
    );

    const ColorProps edge = accent.withAlpha(0.75F * alpha);

    renderer.drawUiRectangle(x + notch, y, width - notch, 1, edge);
    renderer.drawUiRectangle(x, y + notch, 1, height - notch, edge);
    renderer.drawUiRectangle(x, y + height - 1, width * 0.55F, 1, edge);
    renderer.drawUiRectangle(x + width - 1, y, 1, height * 0.45F, edge);

    renderer.drawUiShape(
      QuadKindEnum::solid,
      Vec2Props::of(x + (notch / 2), y + (notch / 2)),
      Vec2Props::of(notch * std::numbers::sqrt2_v<float>, 1),
      edge,
      -ScalarMath::pi / 4.0F,
      0
    );

    renderer.drawUiRectangle(
      x + width - 18,
      y + height - 3,
      14,
      2,
      accent.withAlpha(0.9F * alpha)
    );
  }

  float UiPainter::drawKeycap(
    Renderer        &renderer,
    std::string_view label,
    float            x,
    float            y,
    float            size,
    ColorProps       color
  ) {
    const float width =
      std::max(renderer.measureText(label, size) + (size * 0.9F), size * 1.6F);

    const float height = size * 1.45F;

    renderer.drawUiRectangle(x, y, width, height, color.withAlpha(0.16F));
    renderer.drawUiRectangle(x, y, width, 1, color.withAlpha(0.9F));

    renderer
      .drawUiRectangle(x, y + height - 2, width, 2, color.withAlpha(0.9F));

    renderer.drawUiRectangle(x, y, 1, height, color.withAlpha(0.6F));

    renderer
      .drawUiRectangle(x + width - 1, y, 1, height, color.withAlpha(0.6F));

    renderer.drawText(
      label,
      x + (width / 2),
      y + (size * 0.12F),
      size,
      color,
      TextAlignEnum::center
    );

    return width;
  }

  void UiPainter::drawSegmentedBar(
    Renderer                &renderer,
    const SegmentedBarProps &bar
  ) {
    constexpr float gap = 2;

    for(std::size_t segment = 0; segment < bar.segment_count; ++segment) {
      const float segment_x =
        bar.x + (static_cast<float>(segment) * (bar.segment_width + gap));

      const float threshold = (static_cast<float>(segment) + 0.5F) /
                              static_cast<float>(bar.segment_count);

      renderer.drawUiRectangle(
        segment_x,
        bar.y,
        bar.segment_width,
        bar.height,
        bar.fill >= threshold ? bar.color : bar.empty_color
      );
    }
  }

  void UiPainter::drawGlitchTitle(
    Renderer        &renderer,
    std::string_view text,
    float            x,
    float            y,
    float            size,
    ColorProps       color,
    float            seconds,
    float            intensity
  ) {
    const float jitter =
      (ScalarMath::hashTime(seconds, 23) - 0.5F) * 6 * intensity;

    const float split =
      2.5F + (3 * intensity * ScalarMath::hashTime(seconds + 0.5F, 7));

    renderer.drawDisplayText(
      text,
      x - split + jitter,
      y,
      size,
      Palette::neon_cyan.withAlpha(0.55F * color.a),
      TextAlignEnum::center
    );

    renderer.drawDisplayText(
      text,
      x + split - jitter,
      y + 1,
      size,
      Palette::neon_magenta.withAlpha(0.55F * color.a),
      TextAlignEnum::center
    );

    renderer.drawDisplayText(text, x, y, size, color, TextAlignEnum::center);
  }

  void UiPainter::drawScanlines(
    Renderer &renderer,
    float     width,
    float     height,
    float     alpha
  ) {
    for(int line = 0; static_cast<float>(line) * 4 < height; ++line)
      renderer.drawUiRectangle(
        0,
        static_cast<float>(line) * 4,
        width,
        1,
        ColorProps::black().withAlpha(alpha)
      );
  }
} // namespace wheel_of_fear
