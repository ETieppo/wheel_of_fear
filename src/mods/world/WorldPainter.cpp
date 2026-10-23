#include "mods/world/WorldPainter.hpp"

#include <array>
#include <cmath>
#include <cstdint>

#include "core/math/Scalar.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/Renderer.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/Art.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  WorldPainter::WorldPainter(
    const World    &world,
    GameContextDto &context
  )
      : world_(world)
      , renderer_(context.renderer)
      , sprites_(context.art.sprites())
      , real_seconds_(context.clock.realSeconds()) {
  }

  void WorldPainter::paint() const {
    const TileMap &map = world_.map();
    renderer_.setWallMap(
      static_cast<std::uint32_t>(map.width()),
      static_cast<std::uint32_t>(map.height()),
      map.walls(),
      map.version()
    );
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
    const TileMap            &map    = world_.map();
    const Vec2                origin = renderer_.viewOrigin();
    const TileCoordinateProps first  = TileMap::tileAt(origin);
    const TileCoordinateProps last   = TileMap::tileAt(origin.add(
      Vec2::of(
        static_cast<float>(RenderConfig::view_width),
        static_cast<float>(RenderConfig::view_height)
      )
    ));
    for(int y = first.y - 1; y <= last.y + 1; ++y) {
      for(int x = first.x - 1; x <= last.x + 1; ++x) {
        if(!map.contains(x, y)) continue;
        const std::size_t cell   = map.cellIndex(x, y);
        const Vec2        center = TileMap::tileCenter(x, y);
        const float       ripple =
          1 +
          0.06F *
          std::sin(real_seconds_ * 3 + static_cast<float>(x * 7 + y));
        const float acid = map.acidSeconds()[cell];
        if(acid > 0) {
          const float strength = Scalar::clamp(acid / 4, 0, 1);
          renderer_.drawSprite(
            sprites_.soft_circle,
            center,
            SpriteDrawDto {
              .color = Color::hex(0x5fbf2a).withAlpha(0.55F * strength),
              .scale = 1.1F * ripple
            }
          );
          renderer_.drawGlowDisc(
            center, 9, Palette::neon_lime.withAlpha(0.22F * strength), 1.6F
          );
        }
        const float neon = map.neonSeconds()[cell];
        if(neon <= 0) continue;
        const float         strength = Scalar::clamp(neon / 6, 0, 1);
        const std::uint32_t seed = (static_cast<std::uint32_t>(x) * 73856093U) ^
                                   (static_cast<std::uint32_t>(y) * 19349663U);
        const Vec2          jitter =
          Vec2::of(
            Scalar::hashToUnit(seed) - 0.5F, Scalar::hashToUnit(seed + 1) - 0.5F
          )
            .scale(9);
        const float shimmer =
          0.8F +
          0.2F *
          std::sin(real_seconds_ * 2.5F + Scalar::hashToUnit(seed + 2) * 6.28F);
        const Vec2  blob = center.add(jitter);
        const Color tint = Palette::neon_magenta.mix(
          Palette::neon_violet, Scalar::hashToUnit(seed + 3) * 0.5F
        );
        renderer_.drawSprite(
          sprites_.soft_circle,
          blob,
          SpriteDrawDto {
            .color = tint.withAlpha(0.32F * strength),
            .scale = (1.5F + Scalar::hashToUnit(seed + 4) * 0.5F) * ripple
          }
        );
        renderer_.drawGlowDisc(
          blob, 15, tint.withAlpha(0.12F * strength * shimmer), 2.2F
        );
        if(Scalar::hashToUnit(seed + 5) > 0.7F)
          renderer_.drawGlowDisc(
            blob.add(Vec2::of(-2, -2)),
            2.5F,
            Color::white().withAlpha(0.35F * strength * shimmer),
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
          .color    = Color::hex(0x0d0b0a).withAlpha(0.85F),
          .scale    = 1.5F
        }
      );
    }
  }

  void WorldPainter::paintVents() const {
    for(const Vec2 vent : world_.vents())
      if(renderer_.isVisible(vent, 20))
        renderer_.drawSprite(sprites_.vent, vent);
  }

  void WorldPainter::paintDoorway(const DoorwayProps &doorway) const {
    const Vec2  center   = doorway.center();
    const float rotation = doorway.is_vertical ? Scalar::pi / 2.0F : 0.0F;
    const Vec2  along = doorway.is_vertical ? Vec2::of(0, 1) : Vec2::of(1, 0);
    const float span =
      RenderConfig::tile_size * static_cast<float>(DoorwayProps::length);
    const float slide = doorway.open_amount * span * 0.5F;
    renderer_.drawSprite(
      sprites_.blast_door_frame, center, SpriteDrawDto {.rotation = rotation}
    );
    if(doorway.open_amount < 0.98F) {
      renderer_.drawSprite(
        sprites_.blast_door_panel,
        center.sub(along.scale(span * 0.25F + slide)),
        SpriteDrawDto {.rotation = rotation}
      );
      renderer_.drawSprite(
        sprites_.blast_door_panel,
        center.add(along.scale(span * 0.25F + slide)),
        SpriteDrawDto {.rotation = rotation + Scalar::pi}
      );
    }
    const Color status = doorway.is_open ? Palette::success : Palette::danger;
    const Vec2  side   = doorway.is_vertical ? Vec2::of(1, 0) : Vec2::of(0, 1);
    for(const float sign : std::array<float, 2> {-1, 1}) {
      renderer_.drawGlowDisc(
        center.add(along.scale(span * 0.5F + 3)).add(side.scale(sign * 6)),
        2.2F,
        status.withAlpha(0.9F),
        1.5F
      );
      renderer_.drawGlowDisc(
        center.sub(along.scale(span * 0.5F + 3)).add(side.scale(sign * 6)),
        2.2F,
        status.withAlpha(0.9F),
        1.5F
      );
    }
  }

  void WorldPainter::paintScenery() const {
    for(const auto &item : world_.scenery()) {
      if(!renderer_.isVisible(item->position(), 24)) continue;
      item->draw(
        renderer_,
        sprites_,
        SceneryViewDto {real_seconds_, world_.isRoomPowered(item->roomIndex())}
      );
    }
  }

  void WorldPainter::paintLockers() const {
    for(const Locker &locker : world_.lockers())
      if(renderer_.isVisible(locker.position(), 24))
        locker.draw(renderer_, sprites_);
  }
} // namespace wheel_of_fear
