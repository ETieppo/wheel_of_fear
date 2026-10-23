#pragma once

#include "mods/mobs/Mob.hpp"
#include "mods/world/TileCoordinateProps.hpp"

namespace wheel_of_fear {

class Melter final : public Mob {
public:
    static constexpr MobTraitsProps species_traits{
        .display_name = "Melter",
        .introduction = "MELTER  -  an acid slug. Slow, but walls mean nothing to it. Listen for the hiss.",
        .speed = 22,
        .collision_radius = 8,
        .frozen_by = LightFilterEnum::any_light,
        .sight_factor = 0.8F,
        .hearing_factor = 1.2F,
        .movement_sound = SoundEnum::melter_slither,
        .movement_sound_volume = 0.3F,
        .movement_sound_interval = 1.1F,
        .vocal_sound = SoundEnum::acid_sizzle,
        .vocal_volume = 0.5F,
        .first_phase = 3,
        .spawn_weight = 0.45F,
    };

    explicit Melter(Vec2 position) : Mob(position, species_traits) {}

    MobKindEnum kind() const override { return MobKindEnum::melter; }
    const MobTraitsProps& traits() const override { return species_traits; }
    void emitLights(GameContextDto& context) override;
    void think(GameContextDto& context) override;
    void onContact(GameContextDto& context) override;
    void draw(GameContextDto& context) const override;

private:
    static constexpr float seconds_to_dissolve_wall = 1.8F;
    static constexpr float acid_trail_seconds = 14;
    static constexpr float acid_trail_interval = 0.45F;
    static constexpr float dissolve_noise_radius = 260;
    static constexpr float dissolve_flash_seconds = 0.35F;
    static constexpr float strike_retreat_seconds = 0.8F;

    void dissolveTowards(GameContextDto& context, TileCoordinateProps tile, Vec2 contact_point);
    void leaveAcidTrail(GameContextDto& context);

    float dissolve_seconds_ = 0;
    float trail_seconds_ = 0;
    float flash_seconds_ = 0;
};

}
