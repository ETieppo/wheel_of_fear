#pragma once

#include <string_view>

namespace wheel_of_fear {

class TextureCatalog final {
public:
    TextureCatalog() = delete;

    static constexpr std::string_view face_color = "textures/face_color.png";
    static constexpr std::string_view face_normal = "textures/face_normal.png";
    static constexpr std::string_view floor_color = "textures/floor_color.png";
    static constexpr std::string_view floor_normal = "textures/floor_normal.png";
    static constexpr std::string_view wall_color = "textures/wall_color.png";
    static constexpr std::string_view wall_normal = "textures/wall_normal.png";
};

}
