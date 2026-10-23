#include "core/render/TextureAtlas.hpp"

#include <cstddef>

#include "core/render/RenderConfig.hpp"

namespace wheel_of_fear {

TextureAtlas::TextureAtlas()
    : size_(RenderConfig::atlas_size),
      pixels_(static_cast<std::size_t>(RenderConfig::atlas_size) * RenderConfig::atlas_size * 4, 0) {
    Canvas atlas_canvas = canvas();
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) atlas_canvas.set(x, y, {255, 255, 255, 255});
    }
}

}
