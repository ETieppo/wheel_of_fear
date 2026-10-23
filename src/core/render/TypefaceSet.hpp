#pragma once

#include <array>
#include <string_view>

#include "core/render/FontProps.hpp"
#include "core/render/TextureAtlas.hpp"
#include "core/render/enums/TypefaceEnum.hpp"

namespace wheel_of_fear {

class TypefaceSet {
public:
    void bakeInto(TextureAtlas& atlas);

    const GlyphProps& glyphFor(TypefaceEnum typeface, char32_t codepoint) const;
    float pixelSize(TypefaceEnum typeface) const;
    float measure(TypefaceEnum typeface, std::string_view text, float size) const;

private:
    const FontProps& font(TypefaceEnum typeface) const;

    std::array<FontProps, typeface_count> fonts_{};
};

}
