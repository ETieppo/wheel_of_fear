#include "mods/world/WorldPainter.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/Renderer.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/world/WorldSystem.hpp"
#include <cmath>
#include <cstdint>

namespace wheel_of_fear {
  WorldPainter::WorldPainter(
    const WorldSystem &world,
    GameContextDto    &context
  )
      : world_(world)
      , renderer_(context.renderer)
      , sprites_(context.art.sprites())
      , real_seconds_(context.clock.realSeconds()) {
  }

  void WorldPainter::paint() const {
    const TileMapModel &map = world_.map();

    renderer_.setWallMap(
      static_cast<std::uint32_t>(map.width()),
      static_cast<std::uint32_t>(map.height()),
      map.walls(),
      map.shadowCasters(),
      map.version()
    );

    world_.power().paintRoomGlow(renderer_);
    paintAcidPools();
    paintNeonSplashes();
    paintScorchMarks();
    paintVents();
    world_.power().draw(map, renderer_);

    for(const DoorwayProps &doorway : map.doorways())
      if(renderer_.isVisible(doorway.center(), 40)) paintDoorway(doorway);

    paintScenery();
    paintLockers();
  }

  void WorldPainter::paintAcidPools() const {
    const TileMapModel       &map    = world_.map();
    const Vec2Props           origin = renderer_.viewOrigin();
    const TileCoordinateProps first  = TileMapModel::tileAt(origin);
    const TileCoordinateProps last   = TileMapModel::tileAt(origin.add(
      Vec2Props::of(
        static_cast<float>(RenderConfig::view_width),
        static_cast<float>(RenderConfig::view_height)
      )
    ));

    for(int y = first.y - 1; y <= last.y + 1; ++y) {
      for(int x = first.x - 1; x <= last.x + 1; ++x) {
        if(!map.contains(x, y)) continue;

        const float acid = map.fluids().acidSeconds()[map.cellIndex(x, y)];
        if(acid <= 0) continue;

        const Vec2Props center   = TileMapModel::tileCenter(x, y);
        const float     strength = ScalarMath::clamp(acid / 4, 0, 1);
        const float     ripple   = 1
          + (0.06F
             * std::sin((real_seconds_ * 3) + static_cast<float>((x * 7) + y)));

        renderer_.drawSprite(
          sprites_.soft_circle,
          center,
          SpriteDrawDto {
            .color = ColorProps::hex(0x5fbf2a).withAlpha(0.55F * strength),
            .scale = 1.1F * ripple,
          }
        );

        renderer_.drawGlowDisc(
          center,
          9,
          Palette::neon_lime.withAlpha(0.22F * strength),
          1.6F
        );
      }
    }
  }

  void WorldPainter::paintNeonSplashes() const {
    const TileFluidLayer &fluids = world_.map().fluids();

    for(const NeonSplashProps &splash : world_.neonSplashes()) {
      for(const NeonDropletProps &droplet : splash.droplets) {
        if(!renderer_.isVisible(droplet.position, droplet.radius * 2)) continue;

        const float strength = ScalarMath::clamp(
          fluids.neonAt(droplet.position) / splash.lifetime_seconds,
          0,
          1
        );
        if(strength <= 0) continue;

        const float phase = droplet.seed * ScalarMath::tau;
        const float ripple
          = 1 + (0.06F * std::sin((real_seconds_ * 3) + phase));
        const float shimmer
          = 0.8F + (0.2F * std::sin((real_seconds_ * 2.5F) + phase));
        const float      scale = (droplet.radius / soft_circle_radius) * ripple;
        const ColorProps neon  = splash.color;
        const ColorProps hot
          = neon.mix(ColorProps::white(), neon_core_white_mix);

        renderer_.drawSprite(
          sprites_.soft_circle,
          droplet.position,
          SpriteDrawDto {
            .color = neon.withAlpha(neon_film_opacity * strength),
            .scale = scale,
          }
        );

        renderer_.drawGlowSprite(
          sprites_.soft_circle,
          droplet.position,
          neon.withAlpha(neon_glow_opacity * strength * shimmer),
          SpriteDrawDto {.scale = scale}
        );

        renderer_.drawGlowSprite(
          sprites_.soft_circle,
          droplet.position,
          hot.withAlpha(neon_core_opacity * strength * shimmer),
          SpriteDrawDto {.scale = scale * 0.5F}
        );

        renderer_.drawGlowDisc(
          droplet.position,
          droplet.radius * 1.8F,
          neon.withAlpha(neon_halo_opacity * strength * shimmer),
          2.2F
        );
      }
    }
  }

  void WorldPainter::paintScorchMarks() const {
    for(const ScorchMarkProps &mark : world_.scorchMarks()) {
      if(!renderer_.isVisible(mark.position, 24)) continue;

      renderer_.drawSprite(
        sprites_.scorch[mark.variant],
        mark.position,
        SpriteDrawDto {
          .rotation = mark.rotation,
          .color    = ColorProps::hex(0x0d0b0a).withAlpha(0.85F),
          .scale    = 1.5F,
        }
      );
    }
  }

  void WorldPainter::paintVents() const {
    for(const Vec2Props vent : world_.vents())
      if(renderer_.isVisible(vent, 20))
        renderer_.drawSprite(sprites_.vent, vent);
  }

  void WorldPainter::paintDoorway(const DoorwayProps &doorway) const {
    if(doorway.open_amount <= 0.0F) return;

    const Vec2Props center = doorway.center();
    const float rotation   = doorway.is_vertical ? ScalarMath::pi / 2.0F : 0.0F;
    const Vec2Props along
      = doorway.is_vertical ? Vec2Props::of(0, 1) : Vec2Props::of(1, 0);
    const float span
      = RenderConfig::tile_size * static_cast<float>(DoorwayProps::length);
    const float slide = doorway.open_amount * span * 0.5F;

    renderer_.drawSprite(
      sprites_.blast_door_frame,
      center,
      SpriteDrawDto {.rotation = rotation}
    );

    if(doorway.open_amount < 0.98F) {
      renderer_.drawSprite(
        sprites_.blast_door_panel,
        center.sub(along.scale((span * 0.25F) + slide)),
        SpriteDrawDto {.rotation = rotation}
      );

      renderer_.drawSprite(
        sprites_.blast_door_panel,
        center.add(along.scale((span * 0.25F) + slide)),
        SpriteDrawDto {.rotation = rotation + ScalarMath::pi}
      );
    }
  }

  void WorldPainter::paintScenery() const {
    for(const auto &item : world_.scenery()) {
      if(!renderer_.isVisible(item->position(), 24)) continue;

      item->draw(
        renderer_,
        sprites_,
        SceneryViewDto {
          .real_seconds    = real_seconds_,
          .is_room_powered = world_.isRoomPowered(item->roomIndex()),
        }
      );
    }
  }

  void WorldPainter::paintLockers() const {
    for(const LockerEntity &locker : world_.lockers())
      if(renderer_.isVisible(locker.position(), 24))
        locker.draw(renderer_, sprites_);
  }
} // namespace wheel_of_fear
