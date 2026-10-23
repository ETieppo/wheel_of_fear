#pragma once

#include "mods/mobs/Mob.hpp"

namespace wheel_of_fear {

class Shepherd final : public Mob {
public:
    static constexpr MobTraitsProps species_traits{
        .display_name = "Shepherd",
        .introduction = "SHEPHERD  -  an antlered signal-caller. It keeps its distance and calls the pack to wherever you are.",
        .speed = 46,
        .collision_radius = 8,
        .frozen_by = LightFilterEnum::any_light,
        .sight_factor = 1.25F,
        .field_of_view_cosine = 0.35F,
        .movement_sound = SoundEnum::shepherd_step,
        .movement_sound_volume = 0.34F,
        .movement_sound_interval = 0.7F,
        .first_phase = 3,
        .spawn_weight = 0.3F,
    };

    explicit Shepherd(Vec2 position) : Mob(position, species_traits) {}

    MobKindEnum kind() const override { return MobKindEnum::shepherd; }
    const MobTraitsProps& traits() const override { return species_traits; }
    void think(GameContextDto& context) override;
    void draw(GameContextDto& context) const override;

private:
    static constexpr float preferred_distance = 135;
    static constexpr float call_interval_seconds = 8;
    static constexpr float call_seconds = 1.6F;
    static constexpr float call_radius = 460;
    static constexpr float call_frenzy_seconds = 5;
    static constexpr float call_ripple_seconds = 1.4F;

    void call(GameContextDto& context);

    float seconds_since_call_ = call_interval_seconds;
};

}
