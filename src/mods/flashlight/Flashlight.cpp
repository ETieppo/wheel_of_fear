#include "mods/flashlight/Flashlight.hpp"

#include <algorithm>
#include <cmath>

#include "core/theme/Palette.hpp"

namespace wheel_of_fear {

void Flashlight::reset() {
    mode_ = FlashlightModeEnum::normal;
    is_enabled_ = true;
    is_on_ = true;
    battery_.refill();
}

void Flashlight::cycleMode(SoundPlayer& sound_player) {
    if (!is_enabled_) {
        is_enabled_ = true;
        mode_ = FlashlightModeEnum::normal;
    } else if (mode_ == FlashlightModeEnum::normal) {
        mode_ = FlashlightModeEnum::ultraviolet;
    } else {
        is_enabled_ = false;
    }
    sound_player.play(SoundEnum::switch_click, 0.6F, 0, isUltraviolet() ? 0.8F : 1.1F);
}

void Flashlight::update(const FlashlightConditionsDto& conditions, RandomGenerator& random, SoundPlayer& sound_player) {
    const bool wants_light = is_enabled_ || conditions.is_held_in_mouth;
    float seconds_to_drain = isUltraviolet() ? 75.0F : 190.0F;
    if (conditions.is_held_in_mouth) seconds_to_drain = 240;
    if (wants_light && !conditions.is_tracker_open && !conditions.is_holder_hidden) battery_.drain(conditions.delta_seconds, seconds_to_drain);

    const bool can_charge = !conditions.is_holder_hidden && conditions.is_in_burning_light;
    if (battery_.updateCharging(can_charge, conditions.delta_seconds)) sound_player.play(SoundEnum::battery_charge, 0.5F, 0, 1);

    const bool flickered_off = battery_.flickersOff(random, conditions.delta_seconds);
    const bool was_on = is_on_;
    is_on_ = wants_light && !battery_.isEmpty() && !conditions.is_tracker_open && !conditions.is_holder_hidden && !flickered_off;
    if (was_on && !is_on_ && battery_.isEmpty()) sound_player.play(SoundEnum::electric_buzz, 0.5F, 0, 0.7F);
}

float Flashlight::sightRangeForMobs() const {
    return isUltraviolet() ? 200.0F : 250.0F;
}

Vec2 Flashlight::lampTip(Vec2 holder_position, Vec2 aim_direction) {
    const Vec2 local = Vec2::of(lamp_forward_offset, lamp_side_offset);
    return holder_position.add(Vec2::of(local.x * aim_direction.x - local.y * aim_direction.y, local.x * aim_direction.y + local.y * aim_direction.x));
}

bool Flashlight::usesUltraviolet(const FlashlightGripDto& grip) const {
    return isUltraviolet() && !grip.repair_target;
}

Color Flashlight::beamColor(const FlashlightGripDto& grip) const {
    return usesUltraviolet(grip) ? Palette::ultraviolet_light : Palette::flashlight_white;
}

FlashlightBeamDto Flashlight::beamShape(const FlashlightGripDto& grip) const {
    if (grip.repair_target) {
        const Vec2 toward = grip.repair_target->sub(grip.holder_position);
        const float distance = std::max(toward.length(), 1.0F);
        const Vec2 mouth = grip.holder_position.add(toward.normalized().scale(5));
        const float half_angle = std::atan2(22.0F, distance) + 0.1F;
        return FlashlightBeamDto{mouth, toward.normalized(), distance + 40, std::cos(half_angle)};
    }
    const Vec2 origin = lampTip(grip.holder_position, grip.aim_direction);
    if (isUltraviolet()) return FlashlightBeamDto{origin, grip.aim_direction, 130, 0.83F};
    return FlashlightBeamDto{origin, grip.aim_direction, 170, 0.9F};
}

void Flashlight::emitLights(Lighting& lighting, const FlashlightGripDto& grip) const {
    if (!is_on_) return;
    const FlashlightBeamDto beam = beamShape(grip);
    LightProps beam_light;
    beam_light.position = beam.origin;
    beam_light.radius = beam.range;
    beam_light.intensity = grip.repair_target ? 1.7F : (usesUltraviolet(grip) ? 1.5F : 1.35F);
    beam_light.color = beamColor(grip);
    beam_light.direction = beam.direction;
    beam_light.cone_outer_cosine = beam.cone_outer_cosine;
    beam_light.cone_inner_cosine = beam.cone_outer_cosine + (1 - beam.cone_outer_cosine) * 0.55F;
    beam_light.is_ultraviolet = usesUltraviolet(grip);
    beam_light.flicker = battery_.level() < 0.3F ? 0.25F : 0.03F;
    beam_light.freezes_creatures = true;
    lighting.add(beam_light);
    if (!grip.repair_target) return;
    LightProps pool;
    pool.position = *grip.repair_target;
    pool.radius = repair_pool_radius;
    pool.intensity = 0.55F;
    pool.color = Palette::flashlight_white;
    pool.casts_shadows = false;
    lighting.add(pool);
}

void Flashlight::drawBeam(Renderer& renderer, const SpriteSheetProps& sprites, const FlashlightGripDto& grip) const {
    if (!is_on_) return;
    const FlashlightBeamDto beam = beamShape(grip);
    const Color color = beamColor(grip);
    renderer.drawGlowSprite(sprites.flare, beam.origin, color.withAlpha(0.85F), SpriteDrawDto{.rotation = beam.direction.angle(), .scale = 0.4F});
    if (grip.repair_target) {
        const float distance = grip.repair_target->distance(beam.origin);
        renderer.drawGlowBeam(beam.origin, beam.direction.angle(), distance + 10, 34, color.withAlpha(0.13F));
        renderer.drawGlowDisc(*grip.repair_target, 20, color.withAlpha(0.1F), 1.6F);
        return;
    }
    renderer.drawGlowBeam(beam.origin, beam.direction.angle(), std::min(beam.range, 120.0F), 40, color.withAlpha(0.07F));
}

}
