#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include "game/dtos/GameContextDto.hpp"
#include "mods/director/DemoScenario.hpp"
#include "mods/director/enums/GameStateEnum.hpp"

namespace wheel_of_fear {

class Director {
public:
    void initialize(GameContextDto& context);
    void update(GameContextDto& context);
    void draw(GameContextDto& context) const;

private:
    static constexpr float intro_seconds = 6;
    static constexpr float cleared_seconds = 3;
    static constexpr float game_over_delay_seconds = 1.4F;
    static constexpr std::uint64_t title_seed = 0x7171;

    void enterState(GameStateEnum state);
    void startPhase(GameContextDto& context, std::uint32_t phase);
    void updateTitle(GameContextDto& context);
    void updatePhaseIntro(GameContextDto& context);
    void updatePlaying(GameContextDto& context);
    void updatePaused(GameContextDto& context);
    void updateGameOver(GameContextDto& context);
    void updatePhaseCleared(GameContextDto& context);
    void panTitleCamera(GameContextDto& context) const;
    void mixMusic(GameContextDto& context) const;
    void updateFade(float real_delta_seconds);
    void configurePostProcess(GameContextDto& context) const;

    GameStateEnum state_ = GameStateEnum::title;
    float state_seconds_ = 0;
    std::uint32_t phase_ = 1;
    std::uint64_t seed_counter_ = 0x5EED;
    std::vector<std::string_view> intro_lines_;
    float death_seconds_ = 0;
    float fade_amount_ = 1;
    DemoScenario demo_;
};

}
