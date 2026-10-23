#pragma once

#include <string_view>

namespace wheel_of_fear {

class FontCatalog final {
public:
    FontCatalog() = delete;

    static constexpr std::string_view body_font = "fonts/body_font.ttf";
    static constexpr std::string_view display_font = "fonts/display_font.ttf";
};

}
