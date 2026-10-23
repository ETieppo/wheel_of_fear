#pragma once

#include "core/entity/Entity.hpp"
#include "core/items/enums/ItemEnum.hpp"
#include "core/math/Vec2.hpp"
#include "core/render/Renderer.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/art/SpriteSheetProps.hpp"

namespace wheel_of_fear {

class Projectile : public Entity {
public:
    Projectile(ItemEnum item, Vec2 start, Vec2 target);

    ItemEnum item() const { return item_; }
    Vec2 start() const { return start_; }
    void advance(float delta_seconds);
    bool hasLanded() const { return flight_seconds_ >= total_seconds_; }
    float progress() const;
    Vec2 groundPosition() const;
    Vec2 airPosition() const;
    void draw(Renderer& renderer, const SpriteSheetProps& sprites) const;

    virtual void land(GameContextDto& context, Vec2 landing) = 0;

protected:
    virtual const AtlasRegionProps& sprite(const SpriteSheetProps& sprites) const = 0;
    virtual void drawTrail(Renderer&) const {}

private:
    static constexpr float flight_speed = 200;
    static constexpr float arc_height = 24;

    ItemEnum item_;
    Vec2 start_;
    Vec2 target_;
    float flight_seconds_ = 0;
    float total_seconds_;
};

}
