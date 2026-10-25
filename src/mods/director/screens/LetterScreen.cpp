#include "mods/director/screens/LetterScreen.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/UiTextWrapper.hpp"
#include "core/text/Localization.hpp"
#include "core/theme/Palette.hpp"
#include "mods/director/CampaignConfig.hpp"
#include <cmath>

namespace wheel_of_fear {
  namespace {
    constexpr ColorProps paper_color = ColorProps::hex(0xe9e1cc);
    constexpr ColorProps ink_color   = ColorProps::hex(0x2b2620);
    constexpr ColorProps faded_ink   = ColorProps::hex(0x6d6457);
    constexpr ColorProps gift_ink    = ColorProps::hex(0x8a1f24);

    constexpr LocalizedTextProps letter_heading {
      "LETTER {:02} / {:02}",
      "CARTA {:02} / {:02}",
    };
    constexpr LocalizedTextProps key_note {
      "A small key is taped to the back of the page.",
      "Uma pequena chave está colada no verso da folha.",
    };
    constexpr LocalizedTextProps close_hint {
      "E  /  ENTER  -  KEEP THE KEY AND CLOSE THE LETTER",
      "E  /  ENTER  -  GUARDAR A CHAVE E FECHAR A CARTA",
    };
  } // namespace

  void LetterScreen::draw(
    Renderer        &renderer,
    std::uint32_t    phase,
    std::string_view body,
    std::string_view gift_note,
    float            real_seconds
  ) {
    constexpr float center_x   = RenderConfig::ui_width / 2;
    constexpr float paper_left = center_x - (paper_width / 2);
    constexpr float text_left  = paper_left + paper_padding;
    constexpr float text_width = paper_width - (paper_padding * 2);

    renderer.drawUiRectangle(
      0,
      0,
      RenderConfig::ui_width,
      RenderConfig::ui_height,
      ColorProps::black().withAlpha(0.72F)
    );

    renderer.drawUiRectangle(
      paper_left + 6,
      paper_top + 8,
      paper_width,
      paper_height,
      ColorProps::black().withAlpha(0.5F)
    );

    renderer.drawUiRectangle(
      paper_left,
      paper_top,
      paper_width,
      paper_height,
      paper_color
    );

    renderer.drawText(
      Localization::format(letter_heading, phase, CampaignConfig::final_phase),
      text_left,
      paper_top + 22,
      13,
      faded_ink,
      TextAlignEnum::left
    );

    const float text_size   = fittingTextSize(renderer, body, text_width);
    const float line_height = text_size * line_spacing;
    float       y           = body_top;

    for(
      const std::string_view line :
      UiTextWrapper::wrap(renderer, body, text_size, text_width)
    ) {
      renderer.drawText(
        line,
        text_left,
        y,
        text_size,
        ink_color,
        TextAlignEnum::left
      );
      y += line_height;
    }

    y += line_height * 0.6F;
    renderer.drawText(
      gift_note,
      text_left,
      y,
      text_size,
      gift_ink,
      TextAlignEnum::left
    );
    y += line_height;
    renderer.drawText(
      Localization::text(key_note),
      text_left,
      y,
      text_size - 1,
      faded_ink,
      TextAlignEnum::left
    );

    const float pulse = 0.55F + (0.45F * std::sin(real_seconds * 3));

    renderer.drawText(
      Localization::text(close_hint),
      center_x,
      paper_top + paper_height - 30,
      13,
      faded_ink.withAlpha(pulse),
      TextAlignEnum::center
    );
  }

  float LetterScreen::fittingTextSize(
    const Renderer  &renderer,
    std::string_view body,
    float            text_width
  ) {
    for(float size = largest_text_size; size > smallest_text_size; size -= 1) {
      const auto  lines = UiTextWrapper::wrap(renderer, body, size, text_width);
      const float height
        = static_cast<float>(lines.size() + 3) * size * line_spacing;
      if(body_top + height <= body_bottom) return size;
    }

    return smallest_text_size;
  }
} // namespace wheel_of_fear
