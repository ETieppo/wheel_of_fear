#include "core/render/FontBaker.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "core/assets/AssetLocator.hpp"
#include "core/render/RenderConfig.hpp"

namespace wheel_of_fear {
  namespace {
    struct FreeTypeLibraryDeleter {
        void operator ()(FT_Library library) const {
          FT_Done_FreeType(library);
        }
    };

    struct FreeTypeFaceDeleter {
        void operator ()(FT_Face face) const {
          FT_Done_Face(face);
        }
    };

    using FreeTypeLibrary = std::
      unique_ptr<std::remove_pointer_t<FT_Library>, FreeTypeLibraryDeleter>;
    using FreeTypeFace =
      std::unique_ptr<std::remove_pointer_t<FT_Face>, FreeTypeFaceDeleter>;
  } // namespace

  std::uint32_t FontBaker::bake(
    const std::filesystem::path &font_path,
    FontProps                   &font,
    Canvas                       atlas,
    std::uint32_t                top
  ) {
    const std::vector<std::uint8_t> bytes = AssetLocator::readBytes(font_path);
    FT_Library                      raw_library = nullptr;
    if(FT_Init_FreeType(&raw_library) != 0)
      throw std::runtime_error("unable to initialize FreeType");
    const FreeTypeLibrary library(raw_library);
    FT_Face               raw_face = nullptr;
    if(FT_New_Memory_Face(
         library.get(),
         bytes.data(),
         static_cast<FT_Long>(bytes.size()),
         0,
         &raw_face
       ) != 0) {
      throw std::runtime_error("unable to load font " + font_path.string());
    }
    const FreeTypeFace face(raw_face);
    FT_Set_Pixel_Sizes(face.get(), 0, static_cast<FT_UInt>(font.pixel_size));

    std::uint32_t cursor_x   = 2;
    std::uint32_t cursor_y   = top;
    std::uint32_t row_height = 0;
    for(char32_t codepoint = 32; codepoint < 256; ++codepoint) {
      if(codepoint >= 127 && codepoint < 160) continue;
      const FT_UInt glyph_index = FT_Get_Char_Index(face.get(), codepoint);
      if(glyph_index == 0 && codepoint != 32) continue;
      if(FT_Load_Glyph(face.get(), glyph_index, FT_LOAD_RENDER) != 0) continue;
      const FT_GlyphSlot slot = face->glyph;
      GlyphProps         glyph {};
      glyph.advance              = static_cast<float>(slot->advance.x) / 64.0F;
      glyph.bearing_x            = static_cast<float>(slot->bitmap_left);
      glyph.bearing_y            = static_cast<float>(slot->bitmap_top);
      const std::uint32_t width  = slot->bitmap.width;
      const std::uint32_t height = slot->bitmap.rows;
      if(width > 0 && height > 0) {
        if(cursor_x + width + 2 >= RenderConfig::atlas_size) {
          cursor_x = 2;
          cursor_y += row_height + 2;
          row_height = 0;
        }
        if(cursor_y + height >= RenderConfig::atlas_size)
          throw std::runtime_error("font atlas is full");
        for(std::uint32_t y = 0; y < height; ++y) {
          for(std::uint32_t x = 0; x < width; ++x) {
            const std::uint8_t coverage =
              slot->bitmap.buffer
                [static_cast<std::ptrdiff_t>(y) * slot->bitmap.pitch + x];
            atlas.set(
              static_cast<int>(cursor_x + x),
              static_cast<int>(cursor_y + y),
              {coverage, coverage, coverage, coverage}
            );
          }
        }
        glyph.region = AtlasRegionProps {
          static_cast<float>(cursor_x),
          static_cast<float>(cursor_y),
          static_cast<float>(width),
          static_cast<float>(height),
          1
        };
        cursor_x += width + 2;
        row_height = std::max(row_height, height);
      }
      font.glyphs[codepoint] = glyph;
    }
    return cursor_y + row_height + 4;
  }
} // namespace wheel_of_fear
