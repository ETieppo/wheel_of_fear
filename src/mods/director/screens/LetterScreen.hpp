#pragma once

#include "core/render/Renderer.hpp"
#include <cstdint>
#include <string_view>
#include <vector>

namespace wheel_of_fear {
  class LetterScreen final {
    public:
      LetterScreen() = delete;

      static void draw(
        Renderer        &renderer,
        std::uint32_t    phase,
        std::string_view body,
        std::string_view gift_note,
        float            real_seconds
      );

    private:
      static constexpr float paper_width        = 660;
      static constexpr float paper_top          = 34;
      static constexpr float paper_height       = 472;
      static constexpr float paper_padding      = 34;
      static constexpr float body_top           = 92;
      static constexpr float body_bottom        = 410;
      static constexpr float largest_text_size  = 16;
      static constexpr float smallest_text_size = 12;
      static constexpr float line_spacing       = 1.32F;

      [[nodiscard]]
      static float fittingTextSize(
        const Renderer  &renderer,
        std::string_view body,
        float            text_width
      );
  };
} // namespace wheel_of_fear
