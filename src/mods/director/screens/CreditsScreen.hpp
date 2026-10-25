#pragma once

#include "core/render/Renderer.hpp"
#include "mods/credits/CreditEntryProps.hpp"
#include <span>

namespace wheel_of_fear {
  class CreditsScreen final {
    public:
      CreditsScreen() = delete;

      static void draw(
        Renderer                         &renderer,
        std::span<const CreditEntryProps> entries,
        float                             real_seconds
      );

    private:
      static constexpr float entry_row_height    = 16;
      static constexpr float entry_column_width  = 430;
      static constexpr float entry_section_top   = 222;
      static constexpr float entry_author_offset = 118;

      static void drawEntries(
        Renderer                         &renderer,
        std::span<const CreditEntryProps> entries
      );
  };
} // namespace wheel_of_fear
