#include "mods/art/Art.hpp"

#include <algorithm>
#include <cstring>

#include "core/assets/AssetLocator.hpp"
#include "core/assets/SpriteCatalog.hpp"
#include "core/assets/TextureCatalog.hpp"
#include "core/render/ImageDecoder.hpp"
#include "core/render/RenderConfig.hpp"

namespace wheel_of_fear {
  Art::Art(
    Renderer &renderer
  )
      : renderer_(renderer)
      , packer_(
          RenderConfig::atlas_size,
          RenderConfig::font_region_top
        ) {
    using Catalog             = SpriteCatalog;
    sprites_.player_walk      = loadStrip(Catalog::generated_player_walk);
    sprites_.player_walk_glow = loadStrip(Catalog::generated_player_walk_glow);
    sprites_.player_idle      = loadStrip(Catalog::generated_player_idle);
    sprites_.player_idle_glow = loadStrip(Catalog::generated_player_idle_glow);
    sprites_.player_hug       = loadStrip(Catalog::generated_player_hug);
    sprites_.player_hug_glow  = loadStrip(Catalog::generated_player_hug_glow);
    sprites_.player_repair    = loadStrip(Catalog::generated_player_repair);
    sprites_.player_repair_glow =
      loadStrip(Catalog::generated_player_repair_glow);
    sprites_.crawler_walk = loadStrip(Catalog::generated_crawler_walk);
    sprites_.crawler_walk_glow =
      loadStrip(Catalog::generated_crawler_walk_glow);
    sprites_.bloater_drift = loadStrip(Catalog::generated_bloater_drift);
    sprites_.bloater_drift_glow =
      loadStrip(Catalog::generated_bloater_drift_glow);
    sprites_.melter_crawl = loadStrip(Catalog::generated_melter_crawl);
    sprites_.melter_crawl_glow =
      loadStrip(Catalog::generated_melter_crawl_glow);
    sprites_.shepherd_walk = loadStrip(Catalog::generated_shepherd_walk);
    sprites_.shepherd_walk_glow =
      loadStrip(Catalog::generated_shepherd_walk_glow);
    sprites_.shade_drift      = loadStrip(Catalog::generated_shade_drift);
    sprites_.shade_drift_glow = loadStrip(Catalog::generated_shade_drift_glow);
    sprites_.mimic_hunt       = loadStrip(Catalog::generated_mimic_hunt);
    sprites_.mimic_hunt_glow  = loadStrip(Catalog::generated_mimic_hunt_glow);
    sprites_.moth_flutter     = loadStrip(Catalog::generated_moth_flutter);
    sprites_.moth_flutter_glow =
      loadStrip(Catalog::generated_moth_flutter_glow);
    sprites_.generator_base = loadSingle(Catalog::generated_generator_base);
    sprites_.generator_glow =
      loadSingle(Catalog::generated_generator_base_glow);
    sprites_.generator_rotor = loadSingle(Catalog::generated_generator_rotor);
    sprites_.generator_rotor_glow =
      loadSingle(Catalog::generated_generator_rotor_glow);
    sprites_.fabricator       = loadSingle(Catalog::generated_fabricator);
    sprites_.fabricator_glow  = loadSingle(Catalog::generated_fabricator_glow);
    sprites_.server_rack      = loadSingle(Catalog::generated_server_rack);
    sprites_.server_rack_glow = loadSingle(Catalog::generated_server_rack_glow);
    sprites_.terminal         = loadSingle(Catalog::generated_terminal);
    sprites_.terminal_glow    = loadSingle(Catalog::generated_terminal_glow);
    sprites_.exit_frame       = loadSingle(Catalog::generated_exit_frame);
    sprites_.exit_frame_glow  = loadSingle(Catalog::generated_exit_frame_glow);
    sprites_.blast_door_frame = loadSingle(Catalog::generated_blast_door_frame);
    sprites_.blast_door_panel = loadSingle(Catalog::generated_blast_door_panel);
    sprites_.lure_beacon      = loadSingle(Catalog::generated_lure_beacon);
    sprites_.neon_grenade     = loadSingle(Catalog::generated_neon_grenade);
    sprites_.spark            = loadParticleStrip(Catalog::particle_spark);
    sprites_.light            = loadParticleStrip(Catalog::particle_light);
    sprites_.flare        = loadParticleStrip(Catalog::particle_flare).front();
    sprites_.smoke        = loadParticleStrip(Catalog::particle_smoke);
    sprites_.scorch       = loadParticleStrip(Catalog::particle_scorch);
    sprites_.dirt         = loadParticleStrip(Catalog::particle_dirt);
    sprites_.soft_circle  = loadParticleStrip(Catalog::particle_circle).front();
    sprites_.twirl        = loadParticleStrip(Catalog::particle_twirl).front();
    sprites_.barrel       = loadSingle(Catalog::barrel);
    sprites_.crate        = loadSingle(Catalog::crate);
    sprites_.pipes        = loadSingle(Catalog::pipes);
    sprites_.junction_box = loadSingle(Catalog::junction_box);
    sprites_.locker_closed = loadSingle(Catalog::locker_closed);
    sprites_.locker_open   = loadSingle(Catalog::locker_open);
    sprites_.vent          = loadSingle(Catalog::vent);
    sprites_.neon_signs    = {
      loadSingle(Catalog::neon_sign_0),
      loadSingle(Catalog::neon_sign_1),
      loadSingle(Catalog::neon_sign_2)
    };
    sprites_.item_icons = {
      loadSingle(Catalog::icon_gear),
      loadSingle(Catalog::icon_fuse),
      loadSingle(Catalog::icon_valve),
      loadSingle(Catalog::generated_icon_lure),
      loadSingle(Catalog::generated_icon_grenade),
    };
    renderer_.atlas().markDirty();
    loadGroundMaterials();
  }

  void Art::loadGroundMaterials() {
    GroundMaterialsDto materials;
    materials.floor_color = ImageDecoder::decodeRgba(
      AssetLocator::assetPath(TextureCatalog::floor_color),
      false
    );
    materials.floor_normal = ImageDecoder::decodeRgba(
      AssetLocator::assetPath(TextureCatalog::floor_normal),
      false
    );
    materials.wall_color = ImageDecoder::decodeRgba(
      AssetLocator::assetPath(TextureCatalog::wall_color),
      false
    );
    materials.wall_normal = ImageDecoder::decodeRgba(
      AssetLocator::assetPath(TextureCatalog::wall_normal),
      false
    );
    materials.face_color = ImageDecoder::decodeRgba(
      AssetLocator::assetPath(TextureCatalog::face_color),
      false
    );
    materials.face_normal = ImageDecoder::decodeRgba(
      AssetLocator::assetPath(TextureCatalog::face_normal),
      false
    );
    renderer_.setGroundMaterials(std::move(materials));
  }

  AtlasRegionProps Art::loadSingle(
    const SpriteStripProps &strip
  ) {
    return loadStrip(strip).front();
  }

  SpriteFrames Art::loadParticleStrip(
    const SpriteStripProps &strip
  ) {
    DecodedImageProps image =
      ImageDecoder::decodeRgba(AssetLocator::assetPath(strip.path), false);
    for(std::size_t index = 0; index < image.pixels.size(); index += 4) {
      const std::uint8_t luminance = std::max(
        {image.pixels[index], image.pixels[index + 1], image.pixels[index + 2]}
      );
      const auto coverage = static_cast<std::uint8_t>(
        static_cast<std::uint32_t>(luminance) * image.pixels[index + 3] / 255
      );
      for(std::size_t channel = 0; channel < 4; ++channel)
        image.pixels[index + channel] = coverage;
    }
    return copyFrames(strip, image);
  }

  SpriteFrames Art::loadStrip(
    const SpriteStripProps &strip
  ) {
    const DecodedImageProps image =
      ImageDecoder::decodeRgba(AssetLocator::assetPath(strip.path), true);
    return copyFrames(strip, image);
  }

  SpriteFrames Art::copyFrames(
    const SpriteStripProps  &strip,
    const DecodedImageProps &image
  ) {
    Canvas       atlas = renderer_.atlas().canvas();
    SpriteFrames regions;
    regions.reserve(strip.frame_count);
    for(std::uint32_t frame = 0; frame < strip.frame_count; ++frame) {
      AtlasRegionProps region =
        packer_.reserve(strip.frame_width, strip.frame_height);
      region.pixels_per_unit     = strip.pixels_per_unit;
      const auto        origin_x = static_cast<std::size_t>(region.x);
      const auto        origin_y = static_cast<std::size_t>(region.y);
      const std::size_t row_bytes =
        static_cast<std::size_t>(strip.frame_width) * 4;
      for(std::size_t y = 0; y < strip.frame_height; ++y) {
        const std::size_t source_row = (y *
                                        image.width +
                                        static_cast<std::size_t>(frame) *
                                        strip.frame_width) *
                                       4;
        const std::size_t target_row =
          ((origin_y + y) * RenderConfig::atlas_size + origin_x) * 4;
        std::memcpy(
          atlas.pixels().data() + target_row,
          image.pixels.data() + source_row,
          row_bytes
        );
      }
      regions.push_back(region);
    }
    return regions;
  }
} // namespace wheel_of_fear
