#include "core/render/FontBaker.hpp"
#include <algorithm>
#include <ft2build.h>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include FT_FREETYPE_H
#include "core/assets/AssetLocator.hpp"
#include "core/render/RenderConfig.hpp"

namespace wheel_of_fear {
  namespace {
    class FreeTypeLibraryDeleter {
      public:
        void operator ()(FT_Library library) const {
          FT_Done_FreeType(library);
        }
    };

    class FreeTypeFaceDeleter {
      public:
        void operator ()(FT_Face face) const {
          FT_Done_Face(face);
        }
    };

    using FreeTypeLibrary = std::
      unique_ptr<std::remove_pointer_t<FT_Library>, FreeTypeLibraryDeleter>;
    using FreeTypeFace
      = std::unique_ptr<std::remove_pointer_t<FT_Face>, FreeTypeFaceDeleter>;

    struct GlyphCursorProps {
        std::uint32_t x          = 2;
        std::uint32_t y          = 0;
        std::uint32_t row_height = 0;
    };

    constexpr std::uint32_t glyph_padding = 2;

    FreeTypeLibrary createLibrary() {
      FT_Library raw_library = nullptr;
      if(FT_Init_FreeType(&raw_library) != 0)
        throw std::runtime_error("unable to initialize FreeType");

      return FreeTypeLibrary(raw_library);
    }

    FreeTypeFace loadFace(
      const FreeTypeLibrary           &library,
      const std::vector<std::uint8_t> &bytes,
      const std::filesystem::path     &font_path
    ) {
      FT_Face raw_face = nullptr;

      if(
        FT_New_Memory_Face(
          library.get(),
          bytes.data(),
          static_cast<FT_Long>(bytes.size()),
          0,
          &raw_face
        )
        != 0
      ) {
        throw std::runtime_error("unable to load font " + font_path.string());
      }

      return FreeTypeFace(raw_face);
    }

    void reserveRowSpace(
      GlyphCursorProps &cursor,
      std::uint32_t     width,
      std::uint32_t     height
    ) {
      if(cursor.x + width + glyph_padding >= RenderConfig::atlas_size) {
        cursor.x = glyph_padding;
        cursor.y += cursor.row_height + glyph_padding;
        cursor.row_height = 0;
      }

      if(cursor.y + height >= RenderConfig::atlas_size)
        throw std::runtime_error("font atlas is full");
    }

    void copyBitmap(
      const FT_Bitmap        &bitmap,
      Canvas                 &atlas,
      const GlyphCursorProps &cursor
    ) {
      for(std::uint32_t y = 0; y < bitmap.rows; ++y) {
        for(std::uint32_t x = 0; x < bitmap.width; ++x) {
          const std::uint8_t coverage
            = bitmap
                .buffer[(static_cast<std::ptrdiff_t>(y) * bitmap.pitch) + x];

          atlas.set(
            static_cast<int>(cursor.x + x),
            static_cast<int>(cursor.y + y),
            {coverage, coverage, coverage, coverage}
          );
        }
      }
    }

    GlyphProps bakeGlyph(
      const FT_GlyphSlot slot,
      Canvas            &atlas,
      GlyphCursorProps  &cursor
    ) {
      GlyphProps glyph {};
      glyph.advance   = static_cast<float>(slot->advance.x) / 64.0F;
      glyph.bearing_x = static_cast<float>(slot->bitmap_left);
      glyph.bearing_y = static_cast<float>(slot->bitmap_top);

      const std::uint32_t width  = slot->bitmap.width;
      const std::uint32_t height = slot->bitmap.rows;
      if(width == 0 || height == 0) return glyph;
      reserveRowSpace(cursor, width, height);
      copyBitmap(slot->bitmap, atlas, cursor);

      glyph.region = AtlasRegionProps {
        .x               = static_cast<float>(cursor.x),
        .y               = static_cast<float>(cursor.y),
        .width           = static_cast<float>(width),
        .height          = static_cast<float>(height),
        .pixels_per_unit = 1,
      };

      cursor.x += width + glyph_padding;
      cursor.row_height = std::max(cursor.row_height, height);
      return glyph;
    }
  } // namespace

  std::uint32_t FontBaker::bake(
    const std::filesystem::path &font_path,
    FontProps                   &font,
    Canvas                       atlas,
    std::uint32_t                top
  ) {
    const std::vector<std::uint8_t> bytes = AssetLocator::readBytes(font_path);
    const FreeTypeLibrary           library = createLibrary();
    const FreeTypeFace              face = loadFace(library, bytes, font_path);
    FT_Set_Pixel_Sizes(face.get(), 0, static_cast<FT_UInt>(font.pixel_size));
    GlyphCursorProps cursor {.x = glyph_padding, .y = top, .row_height = 0};

    for(char32_t codepoint = 32; codepoint < 256; ++codepoint) {
      if(codepoint >= 127 && codepoint < 160) continue;
      const FT_UInt glyph_index = FT_Get_Char_Index(face.get(), codepoint);

      if(glyph_index == 0 && codepoint != 32) continue;
      if(FT_Load_Glyph(face.get(), glyph_index, FT_LOAD_RENDER) != 0) continue;
      font.glyphs[codepoint] = bakeGlyph(face->glyph, atlas, cursor);
    }

    return cursor.y + cursor.row_height + 4;
  }
} // namespace wheel_of_fear
