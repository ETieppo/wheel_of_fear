#include "core/render/UiKit.hpp"

#include <algorithm>

#include "core/math/Scalar.hpp"
#include "core/theme/Palette.hpp"

namespace wheel_of_fear {
  namespace {
    constexpr float notch = 7;
  }

  void UiKit::drawPanel(
    Renderer &renderer,
    float     x,
    float     y,
    float     width,
    float     height,
    Color     accent,
    float     alpha
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
    const Color edge = accent.withAlpha(0.75F * alpha);
    renderer.drawUiRectangle(x + notch, y, width - notch, 1, edge);
    renderer.drawUiRectangle(x, y + notch, 1, height - notch, edge);
    renderer.drawUiRectangle(x, y + height - 1, width * 0.55F, 1, edge);
    renderer.drawUiRectangle(x + width - 1, y, 1, height * 0.45F, edge);
    renderer.drawUiShape(
      QuadKindEnum::solid,
      Vec2::of(x + notch / 2, y + notch / 2),
      Vec2::of(notch * 1.414F, 1),
      edge,
      -Scalar::pi / 4.0F,
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

  float UiKit::drawKeycap(
    Renderer        &renderer,
    std::string_view label,
    float            x,
    float            y,
    float            size,
    Color            color
  ) {
    const float width =
      std::max(renderer.measureText(label, size) + size * 0.9F, size * 1.6F);
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
      x + width / 2,
      y + size * 0.12F,
      size,
      color,
      TextAlignEnum::center
    );
    return width;
  }

  void UiKit::drawSegmentedBar(
    Renderer   &renderer,
    float       x,
    float       y,
    std::size_t segment_count,
    float       segment_width,
    float       height,
    float       fill,
    Color       color,
    Color       empty_color
  ) {
    constexpr float gap = 2;
    for(std::size_t segment = 0; segment < segment_count; ++segment) {
      const float segment_x =
        x + static_cast<float>(segment) * (segment_width + gap);
      const float threshold = (static_cast<float>(segment) + 0.5F) /
                              static_cast<float>(segment_count);
      renderer.drawUiRectangle(
        segment_x,
        y,
        segment_width,
        height,
        fill >= threshold ? color : empty_color
      );
    }
  }

  void UiKit::drawGlitchTitle(
    Renderer        &renderer,
    std::string_view text,
    float            x,
    float            y,
    float            size,
    Color            color,
    float            seconds,
    float            intensity
  ) {
    const float jitter = (Scalar::hashTime(seconds, 23) - 0.5F) * 6 * intensity;
    const float split =
      2.5F + 3 * intensity * Scalar::hashTime(seconds + 0.5F, 7);
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

  void UiKit::drawScanlines(
    Renderer &renderer,
    float     width,
    float     height,
    float     alpha
  ) {
    for(float y = 0; y < height; y += 4)
      renderer.drawUiRectangle(0, y, width, 1, Color::black().withAlpha(alpha));
  }
} // namespace wheel_of_fear
