#pragma once

#include "core/entity/Entity.hpp"
#include "core/items/enums/ItemEnum.hpp"
#include "core/math/Vec2Props.hpp"
#include "core/render/Renderer.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/art/SpriteSheetProps.hpp"

namespace wheel_of_fear {
  class Projectile: public Entity {
    public:
      Projectile(
        ItemEnum  item,
        Vec2Props start,
        Vec2Props target
      );

      [[nodiscard]]
      ItemEnum item() const {
        return item_;
      }

      [[nodiscard]]
      Vec2Props start() const {
        return start_;
      }

      void advance(float delta_seconds);

      [[nodiscard]]
      bool hasLanded() const {
        return flight_seconds_ >= total_seconds_;
      }

      [[nodiscard]]
      float progress() const;

      [[nodiscard]]
      Vec2Props groundPosition() const;

      [[nodiscard]]
      Vec2Props airPosition() const;

      void draw(
        Renderer               &renderer,
        const SpriteSheetProps &sprites
      ) const;

      virtual void land(
        GameContextDto &context,
        Vec2Props       landing
      ) = 0;

    protected:
      [[nodiscard]]
      virtual const AtlasRegionProps &
      sprite(const SpriteSheetProps &sprites) const = 0;

      virtual void drawTrail([[maybe_unused]] Renderer &renderer) const {
      }

    private:
      static constexpr float flight_speed = 200;
      static constexpr float arc_height   = 24;

      ItemEnum  item_;
      Vec2Props start_;
      Vec2Props target_;
      float     flight_seconds_ = 0;
      float     total_seconds_;
  };
} // namespace wheel_of_fear
