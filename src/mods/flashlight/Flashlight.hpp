#pragma once

#include "core/audio/SoundPlayer.hpp"
#include "core/math/Color.hpp"
#include "core/math/RandomGenerator.hpp"
#include "core/render/Renderer.hpp"
#include "mods/art/SpriteSheetProps.hpp"
#include "mods/flashlight/FlashlightBattery.hpp"
#include "mods/flashlight/dtos/FlashlightBeamDto.hpp"
#include "mods/flashlight/dtos/FlashlightConditionsDto.hpp"
#include "mods/flashlight/dtos/FlashlightGripDto.hpp"
#include "mods/flashlight/enums/FlashlightModeEnum.hpp"
#include "mods/lighting/Lighting.hpp"

namespace wheel_of_fear {

class Flashlight {
public:
    FlashlightModeEnum mode() const { return mode_; }
    bool isEnabled() const { return is_enabled_; }
    bool isOn() const { return is_on_; }
    const FlashlightBattery& battery() const { return battery_; }
    bool isUltraviolet() const { return mode_ == FlashlightModeEnum::ultraviolet; }

    void reset();
    void setEnabled(bool is_enabled) { is_enabled_ = is_enabled; }
    void cycleMode(SoundPlayer& sound_player);
    void update(const FlashlightConditionsDto& conditions, RandomGenerator& random, SoundPlayer& sound_player);
    void emitLights(Lighting& lighting, const FlashlightGripDto& grip) const;
    void drawBeam(Renderer& renderer, const SpriteSheetProps& sprites, const FlashlightGripDto& grip) const;
    float sightRangeForMobs() const;

    static Vec2 lampTip(Vec2 holder_position, Vec2 aim_direction);

private:
    static constexpr float lamp_forward_offset = 9;
    static constexpr float lamp_side_offset = 3;
    static constexpr float repair_pool_radius = 42;

    bool usesUltraviolet(const FlashlightGripDto& grip) const;
    Color beamColor(const FlashlightGripDto& grip) const;
    FlashlightBeamDto beamShape(const FlashlightGripDto& grip) const;

    FlashlightModeEnum mode_ = FlashlightModeEnum::normal;
    bool is_enabled_ = true;
    bool is_on_ = true;
    FlashlightBattery battery_;
};

}
