#pragma once

#include "mods/mobs/Mob.hpp"

namespace wheel_of_fear {

class Crawler final : public Mob {
public:
    static constexpr MobTraitsProps species_traits{
        .display_name = "Crawler",
        .introduction = "CRAWLER  -  a spider-drone that skitters toward every sound. Light pins it in place.",
        .speed = 84,
        .collision_radius = 7,
        .frozen_by = LightFilterEnum::any_light,
        .field_of_view_cosine = 0.1F,
        .hearing_factor = 1.15F,
        .strike_reach = 11,
        .movement_sound = SoundEnum::crawler_skitter,
        .movement_sound_volume = 0.26F,
        .movement_sound_interval = 0.34F,
        .vocal_sound = SoundEnum::crawler_chitter,
        .vocal_volume = 0.55F,
        .first_phase = 1,
        .spawn_weight = 1.0F,
    };

    explicit Crawler(Vec2 position) : Mob(position, species_traits) {}

    MobKindEnum kind() const override { return MobKindEnum::crawler; }
    const MobTraitsProps& traits() const override { return species_traits; }
    void onContact(GameContextDto& context) override;
    void onLightReleased(GameContextDto& context) override;
    void draw(GameContextDto& context) const override;

private:
    static constexpr float strike_retreat_seconds = 1.4F;
    static constexpr float flinch_seconds = 0.7F;
};

}
