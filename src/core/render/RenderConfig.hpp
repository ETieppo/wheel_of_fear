#pragma once

#include <cstddef>
#include <cstdint>

namespace wheel_of_fear {

class RenderConfig final {
public:
    RenderConfig() = delete;

    static constexpr std::uint32_t view_width = 416;
    static constexpr std::uint32_t view_height = 234;
    static constexpr float ui_width = 960;
    static constexpr float ui_height = 540;
    static constexpr std::uint32_t atlas_size = 4096;
    static constexpr std::uint32_t font_region_top = 3456;
    static constexpr std::size_t max_gpu_lights = 32;
    static constexpr std::uint32_t max_map_size = 160;
    static constexpr float tile_size = 16;
    static constexpr std::uint32_t light_resolution_divisor = 2;
    static constexpr std::uint32_t bloom_resolution_divisor = 4;
    static constexpr std::size_t world_quad_capacity = 16384;
    static constexpr std::size_t glow_quad_capacity = 8192;
    static constexpr std::size_t ui_quad_capacity = 8192;
};

}
