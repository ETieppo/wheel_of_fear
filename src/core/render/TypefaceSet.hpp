#pragma once

#include "core/render/FontProps.hpp"
#include "core/render/TextureAtlas.hpp"
#include "core/render/enums/TypefaceEnum.hpp"
#include <array>
#include <string_view>

namespace wheel_of_fear {
  class TypefaceSet {
    public:
      void bakeInto(TextureAtlas &atlas);

      [[nodiscard]]
      const GlyphProps &glyphFor(
        TypefaceEnum typeface,
        char32_t     codepoint
      ) const;

      [[nodiscard]]
      float pixelSize(TypefaceEnum typeface) const;

      [[nodiscard]]
      float measure(
        TypefaceEnum     typeface,
        std::string_view text,
        float            size
      ) const;

    private:
      [[nodiscard]]
      const FontProps &font(TypefaceEnum typeface) const;

      std::array<FontProps, typeface_count> fonts_ {};
  };
} // namespace wheel_of_fear
