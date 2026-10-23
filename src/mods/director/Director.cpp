#include "mods/director/Director.hpp"

#include <cmath>
#include <format>

#include "core/audio/SoundPlayer.hpp"
#include "core/input/Input.hpp"
#include "core/math/Scalar.hpp"
#include "core/render/Renderer.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/camera/Camera.hpp"
#include "mods/camera/RoomFraming.hpp"
#include "mods/director/screens/CenteredCard.hpp"
#include "mods/director/screens/PhaseIntroScreen.hpp"
#include "mods/director/screens/TitleScreen.hpp"
#include "mods/distractions/Distractions.hpp"
#include "mods/effects/Effects.hpp"
#include "mods/generators/Generators.hpp"
#include "mods/hud/Hud.hpp"
#include "mods/mobs/Mobs.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/player/Player.hpp"
#include "mods/throwables/Throwables.hpp"
#include "mods/tracker/Tracker.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {

namespace {

constexpr Color ambient_darkness{0.010F, 0.011F, 0.020F, 1};
constexpr Color title_ambient{0.05F, 0.055F, 0.09F, 1};

}

void Director::initialize(GameContextDto& context) {
    World& world = context.world;
    world.generatePhase(1, title_seed);
    for (std::size_t room_index = 0; room_index < world.map().rooms().size(); ++room_index) {
        if (room_index % 3 == 0) world.powerRoom(room_index, true);
    }
    context.camera.setFocus(world.map().rooms().front().center());
}

void Director::enterState(GameStateEnum state) {
    state_ = state;
    state_seconds_ = 0;
}

void Director::startPhase(GameContextDto& context, std::uint32_t phase) {
    World& world = context.world;
    phase_ = phase;
    seed_counter_ += 0x9E3779B97F4A7C15ULL;
    world.generatePhase(phase, seed_counter_);
    context.generators.generatePhase(world);
    context.mobs.populatePhase(world);
    context.throwables.generatePhase(world);
    context.distractions.clear();
    context.noise.clear();
    context.player.respawn(world.playerStart());
    context.effects.clear();
    context.tracker.close();
    context.hud.clearMessages();
    context.camera.snapToPlayer(world, context.player);
    intro_lines_ = Mobs::introductionsForPhase(phase);
    death_seconds_ = 0;
    demo_.restart();
    enterState(GameStateEnum::phase_intro);
    context.sound_player.play(SoundEnum::phase_start, 0.9F, 0, 1);
}

void Director::update(GameContextDto& context) {
    state_seconds_ += context.clock.realDeltaSeconds();
    if (context.input.wasPressed(InputActionEnum::toggle_mute)) context.sound_player.toggleMute();
    mixMusic(context);
    switch (state_) {
    case GameStateEnum::title: updateTitle(context); break;
    case GameStateEnum::phase_intro: updatePhaseIntro(context); break;
    case GameStateEnum::playing: updatePlaying(context); break;
    case GameStateEnum::paused: updatePaused(context); break;
    case GameStateEnum::game_over: updateGameOver(context); break;
    case GameStateEnum::phase_cleared: updatePhaseCleared(context); break;
    }
    updateFade(context.clock.realDeltaSeconds());
}

void Director::updateTitle(GameContextDto& context) {
    context.clock.setRunning(false);
    context.hud.setVisible(false);
    panTitleCamera(context);
    if (!context.input.wasPressed(InputActionEnum::confirm) && !demo_.isActive()) return;
    context.sound_player.play(SoundEnum::ui_glitch, 0.8F, 0, 1);
    startPhase(context, demo_.startingPhase());
}

void Director::updatePhaseIntro(GameContextDto& context) {
    context.clock.setRunning(false);
    context.hud.setVisible(true);
    const bool is_skipped = context.input.wasPressed(InputActionEnum::confirm) || (demo_.isActive() && state_seconds_ > 0.5F);
    if (state_seconds_ <= intro_seconds && !is_skipped) return;
    enterState(GameStateEnum::playing);
    context.clock.setRunning(true);
}

void Director::updatePlaying(GameContextDto& context) {
    context.clock.setRunning(true);
    if (demo_.isActive()) demo_.run(context, state_seconds_);
    if (context.input.wasPressed(InputActionEnum::back)) {
        enterState(GameStateEnum::paused);
        context.clock.setRunning(false);
        return;
    }
    if (context.player.isDead()) {
        death_seconds_ += context.clock.realDeltaSeconds();
        if (death_seconds_ > game_over_delay_seconds) {
            enterState(GameStateEnum::game_over);
            context.clock.setRunning(false);
        }
    }
    if (context.generators.hasPlayerExited()) {
        enterState(GameStateEnum::phase_cleared);
        context.clock.setRunning(false);
        context.sound_player.play(SoundEnum::generator_online, 0.8F, 0, 0.7F);
    }
}

void Director::updatePaused(GameContextDto& context) {
    context.clock.setRunning(false);
    if (context.input.wasPressed(InputActionEnum::back) || context.input.wasPressed(InputActionEnum::confirm)) {
        enterState(GameStateEnum::playing);
        context.clock.setRunning(true);
    }
    if (context.input.wasPressed(InputActionEnum::restart)) startPhase(context, phase_);
}

void Director::updateGameOver(GameContextDto& context) {
    context.clock.setRunning(false);
    if (context.input.wasPressed(InputActionEnum::restart) || context.input.wasPressed(InputActionEnum::confirm)) startPhase(context, phase_);
    if (context.input.wasPressed(InputActionEnum::back)) {
        enterState(GameStateEnum::title);
        context.camera.stopFollowing();
    }
}

void Director::updatePhaseCleared(GameContextDto& context) {
    context.clock.setRunning(false);
    if (state_seconds_ > cleared_seconds) startPhase(context, phase_ + 1);
}

void Director::updateFade(float real_delta_seconds) {
    float target_fade = 0;
    if (state_ == GameStateEnum::phase_cleared) target_fade = Scalar::clamp(state_seconds_ / cleared_seconds, 0, 1);
    if (state_ == GameStateEnum::phase_intro) target_fade = Scalar::clamp(1 - state_seconds_ / 1.2F, 0, 1);
    fade_amount_ = Scalar::approach(fade_amount_, target_fade, real_delta_seconds * 3);
}

void Director::panTitleCamera(GameContextDto& context) const {
    const auto& rooms = context.world.map().rooms();
    if (rooms.empty()) return;
    const std::size_t room_index = static_cast<std::size_t>(state_seconds_ / 9) % rooms.size();
    const RoomProps& room = rooms[room_index];
    const Vec2 drift = Vec2::of(std::cos(state_seconds_ * 0.3F) * 50, std::sin(state_seconds_ * 0.4F) * 30);
    context.camera.glideTo(RoomFraming::clampToRoom(room, room.center().add(drift)), context.clock.realDeltaSeconds());
}

void Director::mixMusic(GameContextDto& context) const {
    SoundPlayer& sound_player = context.sound_player;
    switch (state_) {
    case GameStateEnum::title: sound_player.setMusicMix(1, 0, 0); break;
    case GameStateEnum::phase_intro:
    case GameStateEnum::phase_cleared: sound_player.setMusicMix(0, 0.8F, 0); break;
    case GameStateEnum::playing:
    case GameStateEnum::paused: sound_player.setMusicMix(0, 0.8F, Scalar::clamp(context.mobs.dangerLevel() * 1.4F, 0, 1)); break;
    case GameStateEnum::game_over: sound_player.setMusicMix(0, 0.5F, 0); break;
    }
    sound_player.updateMusic(context.clock.realDeltaSeconds());
}

void Director::configurePostProcess(GameContextDto& context) const {
    const Player& player = context.player;
    const Flashlight& flashlight = player.flashlight();
    const float ultraviolet_tint = flashlight.isUltraviolet() && flashlight.isOn() && !player.isRepairing() ? 1.0F : 0.0F;
    const float desaturation = state_ == GameStateEnum::game_over || player.isDead() ? Scalar::clamp(death_seconds_, 0, 0.85F) : 0.0F;
    const float danger = context.clock.isRunning() ? context.mobs.dangerLevel() : 0.0F;
    const bool is_in_play = state_ != GameStateEnum::title;
    context.renderer.setPostProcess(is_in_play ? ambient_darkness : title_ambient, {danger, player.vitality().hurtFlash(), context.tracker.openAmount(), fade_amount_}, {ultraviolet_tint, 0, desaturation, 0});
    context.renderer.setViewer(player.position(), is_in_play && !player.isHidden() && context.world.map().width() > 0);
}

void Director::draw(GameContextDto& context) const {
    configurePostProcess(context);
    Renderer& renderer = context.renderer;
    const float real_seconds = context.clock.realSeconds();
    switch (state_) {
    case GameStateEnum::title: TitleScreen::draw(renderer, real_seconds); break;
    case GameStateEnum::phase_intro: PhaseIntroScreen::draw(renderer, phase_, intro_lines_, state_seconds_, intro_seconds, real_seconds); break;
    case GameStateEnum::paused: CenteredCard::draw(renderer, "PAUSED", "ESC  RESUME     R  RESTART SECTOR     M  MUTE", Palette::panel_edge, real_seconds); break;
    case GameStateEnum::game_over: CenteredCard::draw(renderer, "SIGNAL LOST", "R  TRY AGAIN     ESC  TITLE SCREEN", Palette::danger, real_seconds); break;
    case GameStateEnum::phase_cleared:
        CenteredCard::draw(renderer, std::format("SECTOR {:02} CLEARED", phase_), "DESCENDING DEEPER INTO THE GRID...", Palette::success, real_seconds);
        break;
    case GameStateEnum::playing: break;
    }
}

}
