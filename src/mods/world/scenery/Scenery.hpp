#pragma once

#include <cstddef>
#include <optional>

#include "core/math/Color.hpp"
#include "core/math/RandomGenerator.hpp"
#include "core/math/Vec2.hpp"
#include "core/render/AtlasRegionProps.hpp"
#include "core/render/Renderer.hpp"
#include "mods/art/SpriteSheetProps.hpp"
#include "mods/distractions/enums/DistractionKindEnum.hpp"
#include "mods/lighting/Lighting.hpp"
#include "mods/world/dtos/SceneryPlacementDto.hpp"
#include "mods/world/dtos/SceneryViewDto.hpp"

namespace wheel_of_fear {

class Scenery {
public:
    explicit Scenery(const SceneryPlacementDto& placement);
    virtual ~Scenery() = default;

    Vec2 position() const { return position_; }
    std::size_t roomIndex() const { return room_index_; }
    bool isBroken() const { return is_broken_; }

    void update(float delta_seconds);
    void bump(RandomGenerator& random);

    virtual std::optional<DistractionKindEnum> bumpDistraction() const = 0;
    virtual void draw(Renderer& renderer, const SpriteSheetProps& sprites, const SceneryViewDto& view) const;
    virtual void emitLight(Lighting&, const SceneryViewDto&) const {}

protected:
    virtual const AtlasRegionProps& sprite(const SpriteSheetProps& sprites) const = 0;
    virtual const AtlasRegionProps* glowSprite(const SpriteSheetProps&) const { return nullptr; }
    virtual void reactToBump(RandomGenerator&) {}

    Color color() const { return color_; }
    bool isMirrored() const { return mirrored_; }
    float flickerSeed() const { return flicker_seed_; }
    void breakApart() { is_broken_ = true; }
    void tilt(float amount) { tilt_ = amount; }

private:
    static constexpr float wobble_duration_seconds = 0.8F;

    Vec2 position_;
    bool mirrored_;
    std::size_t room_index_;
    Color color_;
    float flicker_seed_;
    bool is_broken_ = false;
    float wobble_seconds_ = 0;
    float tilt_ = 0;
};

}
