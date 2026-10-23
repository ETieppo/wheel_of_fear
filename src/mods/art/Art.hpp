#pragma once

#include "core/assets/SpriteStripProps.hpp"
#include "core/render/DecodedImageProps.hpp"
#include "core/render/Renderer.hpp"
#include "mods/art/AtlasShelfPacker.hpp"
#include "mods/art/SpriteSheetProps.hpp"

namespace wheel_of_fear {
  class Art {
    public:
      explicit Art(Renderer &renderer);

      const SpriteSheetProps &sprites() const {
        return sprites_;
      }

    private:
      SpriteFrames     loadStrip(const SpriteStripProps &strip);
      SpriteFrames     loadParticleStrip(const SpriteStripProps &strip);
      AtlasRegionProps loadSingle(const SpriteStripProps &strip);
      SpriteFrames     copyFrames(
        const SpriteStripProps  &strip,
        const DecodedImageProps &image
      );
      void loadGroundMaterials();

      Renderer        &renderer_;
      AtlasShelfPacker packer_;
      SpriteSheetProps sprites_;
  };
} // namespace wheel_of_fear
