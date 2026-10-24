#include "core/render/TypefaceSet.hpp"

#include <cstddef>
#include <exception>

#include <SDL3/SDL_log.h>

#include "core/assets/AssetLocator.hpp"
#include "core/assets/FontCatalog.hpp"
#include "core/render/FontBaker.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/Utf8Decoder.hpp"

namespace wheel_of_fear {
  void TypefaceSet::bakeInto(
    TextureAtlas &atlas
  ) {
    std::uint32_t cursor_y = RenderConfig::font_region_top;
    try {
      cursor_y = FontBaker::bake(
        AssetLocator::assetPath(FontCatalog::body_font),
        fonts_[static_cast<std::size_t>(TypefaceEnum::body)],
        atlas.canvas(),
        cursor_y
      );
    } catch(const std::exception &error) {
      SDL_Log("body font unavailable: %s", error.what());
    }
    try {
      FontBaker::bake(
        AssetLocator::assetPath(FontCatalog::display_font),
        fonts_[static_cast<std::size_t>(TypefaceEnum::display)],
        atlas.canvas(),
        cursor_y
      );
    } catch(const std::exception &error) {
      SDL_Log("display font unavailable: %s", error.what());
    }
    atlas.markDirty();
  }

  const FontProps &TypefaceSet::font(
    TypefaceEnum typeface
  ) const {
    return fonts_[static_cast<std::size_t>(typeface)];
  }

  const GlyphProps &TypefaceSet::glyphFor(
    TypefaceEnum typeface,
    char32_t     codepoint
  ) const {
    return font(typeface).glyphs[codepoint < 256 ? codepoint : U'?'];
  }

  float TypefaceSet::pixelSize(
    TypefaceEnum typeface
  ) const {
    return font(typeface).pixel_size;
  }

  float TypefaceSet::measure(
    TypefaceEnum     typeface,
    std::string_view text,
    float            size
  ) const {
    const float scale  = size / pixelSize(typeface);
    float       width  = 0;
    std::size_t cursor = 0;
    while(cursor < text.size())
      width +=
        glyphFor(typeface, Utf8Decoder::nextCodepoint(text, cursor)).advance *
        scale;
    return width;
  }
} // namespace wheel_of_fear
