#pragma once

#include "core/render/Canvas.hpp"
#include <cstdint>
#include <span>
#include <vector>

namespace wheel_of_fear {
  class TextureAtlas {
    public:
      TextureAtlas();

      Canvas canvas() {
        return {pixels_, size_};
      }

      [[nodiscard]]
      std::span<const std::uint8_t> pixels() const {
        return pixels_;
      }

      [[nodiscard]]
      std::uint32_t size() const {
        return size_;
      }

      [[nodiscard]]
      bool needsUpload() const {
        return needs_upload_;
      }

      void markDirty() {
        needs_upload_ = true;
      }

      void markUploaded() {
        needs_upload_ = false;
      }

    private:
      std::uint32_t             size_;
      std::vector<std::uint8_t> pixels_;
      bool                      needs_upload_ = true;
  };
} // namespace wheel_of_fear
