#pragma once

#include "core/assets/SpriteStripProps.hpp"
#include "core/render/DecodedImageProps.hpp"
#include "core/render/Renderer.hpp"
#include "core/render/dtos/GroundMaterialsDto.hpp"
#include "mods/art/AtlasShelfPacker.hpp"
#include "mods/art/SpriteSheetProps.hpp"
#include <string_view>

namespace wheel_of_fear {
  class ArtLibrary {
    public:
      explicit ArtLibrary(Renderer &renderer);

      [[nodiscard]]
      const SpriteSheetProps &sprites() const {
        return sprites_;
      }

      void useFacilityGround();
      void useOutdoorGround();

    private:
      SpriteFrames     loadStrip(const SpriteStripProps &strip);
      SpriteFrames     loadParticleStrip(const SpriteStripProps &strip);
      AtlasRegionProps loadSingle(const SpriteStripProps &strip);

      static GroundMaterialsDto decodeGroundMaterials(
        std::string_view floor_color_path,
        std::string_view floor_normal_path
      );

      Renderer        &renderer_;
      AtlasShelfPacker packer_;
      SpriteSheetProps sprites_;

      SpriteFrames copyFrames(
        const SpriteStripProps  &strip,
        const DecodedImageProps &image
      );
  };
} // namespace wheel_of_fear
