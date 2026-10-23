#pragma once

#include <cstdint>
#include <span>
#include <vector>

#include "core/render/Canvas.hpp"

namespace wheel_of_fear {

class TextureAtlas {
public:
    TextureAtlas();

    Canvas canvas() { return Canvas(pixels_, size_); }
    std::span<const std::uint8_t> pixels() const { return pixels_; }
    std::uint32_t size() const { return size_; }
    bool needsUpload() const { return needs_upload_; }
    void markDirty() { needs_upload_ = true; }
    void markUploaded() { needs_upload_ = false; }

private:
    std::uint32_t size_;
    std::vector<std::uint8_t> pixels_;
    bool needs_upload_ = true;
};

}
