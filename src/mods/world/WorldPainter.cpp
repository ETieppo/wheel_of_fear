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
      map.version()
    );

    world_.power().paintRoomGlow(renderer_);
    paintFloorLiquids();
    paintScorchMarks();
    paintVents();
    world_.power().draw(map, renderer_);

    for(const DoorwayProps &doorway : map.doorways())
      if(renderer_.isVisible(doorway.center(), 40)) paintDoorway(doorway);

    paintScenery();
    paintLockers();
  }

  void WorldPainter::paintFloorLiquids() const {
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

        const std::size_t cell   = map.cellIndex(x, y);
        const Vec2Props   center = TileMapModel::tileCenter(x, y);
        const float       ripple = 1
          + (0.06F
             * std::sin((real_seconds_ * 3) + static_cast<float>((x * 7) + y)));
        const float acid = map.fluids().acidSeconds()[cell];

        if(acid > 0) {
          const float strength = ScalarMath::clamp(acid / 4, 0, 1);

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

        const float neon = map.fluids().neonSeconds()[cell];
        if(neon <= 0) continue;

        const float         strength = ScalarMath::clamp(neon / 6, 0, 1);
        const std::uint32_t seed = (static_cast<std::uint32_t>(x) * 73856093U)
          ^ (static_cast<std::uint32_t>(y) * 19349663U);
        const Vec2Props jitter  = Vec2Props::of(
                                    ScalarMath::hashToUnit(seed) - 0.5F,
                                    ScalarMath::hashToUnit(seed + 1) - 0.5F
        )
                                    .scale(9);
        const float     shimmer = 0.8F
          + (0.2F
             * std::sin(
               (real_seconds_ * 2.5F)
               + (ScalarMath::hashToUnit(seed + 2) * 6.28F)
             ));
        const Vec2Props  blob = center.add(jitter);
        const ColorProps tint = Palette::neon_magenta.mix(
          Palette::neon_violet,
          ScalarMath::hashToUnit(seed + 3) * 0.5F
        );

        renderer_.drawSprite(
          sprites_.soft_circle,
          blob,
          SpriteDrawDto {
            .color = tint.withAlpha(0.32F * strength),
            .scale
            = (1.5F + (ScalarMath::hashToUnit(seed + 4) * 0.5F)) * ripple,
          }
        );

        renderer_.drawGlowDisc(
          blob,
          15,
          tint.withAlpha(0.12F * strength * shimmer),
          2.2F
        );

        if(ScalarMath::hashToUnit(seed + 5) > 0.7F)
          renderer_.drawGlowDisc(
            blob.add(Vec2Props::of(-2, -2)),
            2.5F,
            ColorProps::white().withAlpha(0.35F * strength * shimmer),
            1.5F
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
