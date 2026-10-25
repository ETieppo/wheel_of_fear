#include "mods/director/Director.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/input/InputController.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/Renderer.hpp"
#include "core/time/GameClock.hpp"
#include "mods/camera/CameraSystem.hpp"
#include "mods/distractions/DistractionSystem.hpp"
#include "mods/effects/EffectSystem.hpp"
#include "mods/extra_lives/ExtraLifeSystem.hpp"
#include "mods/generators/GeneratorSystem.hpp"
#include "mods/hud/HudSystem.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/noise/NoiseSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/throwables/ThrowableSystem.hpp"
#include "mods/tracker/TrackerSystem.hpp"
#include "mods/world/WorldSystem.hpp"
#include <utility>

namespace wheel_of_fear {
  namespace {
    constexpr ColorProps
      ambient_darkness {.r = 0.010F, .g = 0.011F, .b = 0.020F, .a = 1};
    constexpr ColorProps
      title_ambient {.r = 0.05F, .g = 0.055F, .b = 0.09F, .a = 1};
  } // namespace

  Director::Director(std::optional<std::filesystem::path> progress_file_path)
      : progress_(
          demo_.isActive() ? std::nullopt : std::move(progress_file_path)
        ) {
  }

  void Director::initialize(GameContextDto &context) {
    WorldSystem &world = context.world;
    world.generatePhase(1, title_seed);

    for(
      std::size_t room_index = 0; room_index < world.map().rooms().size();
      ++room_index
    )
      if(room_index % 3 == 0) world.powerRoom(room_index, true);

    context.camera.setFocus(world.map().rooms().front().center());
  }

  DirectorSnapshotProps Director::snapshot() const {
    return DirectorSnapshotProps {
      .is_in_phase       = state_ != GameStateEnum::title,
      .phase             = phase_,
      .phase_seed        = seed_counter_,
      .player_max_health = committed_max_health_,
    };
  }

  void Director::resume(
    GameContextDto              &context,
    const DirectorSnapshotProps &snapshot
  ) {
    if(!snapshot.is_in_phase) return;
    seed_counter_         = snapshot.phase_seed - phase_seed_step;
    committed_max_health_ = snapshot.player_max_health;
    startPhase(context, snapshot.phase);
  }

  void Director::enterState(GameStateEnum state) {
    state_         = state;
    state_seconds_ = 0;
  }

  void Director::startPhase(
    GameContextDto &context,
    std::uint32_t   phase
  ) {
    WorldSystem &world = context.world;
    phase_             = phase;
    seed_counter_ += phase_seed_step;
    world.generatePhase(phase, seed_counter_);
    context.generators.generatePhase(world);
    context.mobs.populatePhase(world);
    context.throwables.generatePhase(world);
    context.distractions.clear();
    context.noise.clear();
    context.extra_lives.generatePhase(
      world,
      seed_counter_,
      committed_max_health_ < VitalityComponent::highest_max_health
    );
    context.player.vitality().setMaxHealth(committed_max_health_);
    context.player.respawn(world.playerStart());
    context.effects.clear();
    context.tracker.close();
    context.hud.clearMessages();
    context.camera.snapToPlayer(world, context.player);
    death_seconds_ = 0;
    demo_.restart();
    enterState(GameStateEnum::phase_intro);
    context.sound_player.play(SoundEnum::phase_start, 0.9F, 0, 1);
  }

  void Director::startNewRun(
    GameContextDto &context,
    std::uint32_t   phase
  ) {
    committed_max_health_ = VitalityComponent::base_max_health;
    startPhase(context, phase);
  }

  void Director::advanceToNextPhase(GameContextDto &context) {
    committed_max_health_ = context.player.vitality().maxHealth();
    startPhase(context, phase_ + 1);
    saveProgress();
  }

  void Director::saveProgress() {
    progress_.save(snapshot());
  }

  void Director::update(GameContextDto &context) {
    state_seconds_ += context.clock.realDeltaSeconds();
    if(context.input.wasPressed(InputActionEnum::toggle_mute))
      context.sound_player.toggleMute();

    currentState().mixMusic(context);
    context.sound_player.updateMusic(context.clock.realDeltaSeconds());
    currentState().update(*this, context);
    updateFade(context.clock.realDeltaSeconds());
  }

  const GameState &Director::currentState() const {
    switch(state_) {
      case GameStateEnum::title:         return title_state_;
      case GameStateEnum::phase_intro:   return phase_intro_state_;
      case GameStateEnum::playing:       return playing_state_;
      case GameStateEnum::paused:        return paused_state_;
      case GameStateEnum::game_over:     return game_over_state_;
      case GameStateEnum::phase_cleared: return phase_cleared_state_;
      case GameStateEnum::credits:       return credits_state_;
    }

    return title_state_;
  }

  GameState &Director::currentState() {
    switch(state_) {
      case GameStateEnum::title:         return title_state_;
      case GameStateEnum::phase_intro:   return phase_intro_state_;
      case GameStateEnum::playing:       return playing_state_;
      case GameStateEnum::paused:        return paused_state_;
      case GameStateEnum::game_over:     return game_over_state_;
      case GameStateEnum::phase_cleared: return phase_cleared_state_;
      case GameStateEnum::credits:       return credits_state_;
    }

    return title_state_;
  }

  void Director::updateFade(float real_delta_seconds) {
    float target_fade = 0;

    if(state_ == GameStateEnum::phase_cleared)
      target_fade = ScalarMath::clamp(state_seconds_ / cleared_seconds, 0, 1);
    if(state_ == GameStateEnum::phase_intro)
      target_fade = ScalarMath::clamp(1 - (state_seconds_ / 1.2F), 0, 1);

    fade_amount_
      = ScalarMath::approach(fade_amount_, target_fade, real_delta_seconds * 3);
  }

  void Director::configurePostProcess(GameContextDto &context) const {
    const PlayerEntity        &player     = context.player;
    const FlashlightComponent &flashlight = player.flashlight();
    const float                ultraviolet_tint
      = flashlight.isUltraviolet() && flashlight.isOn() && !player.isRepairing()
      ? 1.0F
      : 0.0F;
    const float desaturation
      = state_ == GameStateEnum::game_over || player.isDead()
      ? ScalarMath::clamp(death_seconds_, 0, 0.85F)
      : 0.0F;
    const float danger
      = context.clock.isRunning() ? context.mobs.danger().level() : 0.0F;
    const bool is_in_play = state_ != GameStateEnum::title;

    context.renderer.setPostProcess(
      is_in_play ? ambient_darkness : title_ambient,
      {
        danger,
        player.vitality().hurtFlash(),
        context.tracker.openAmount(),
        fade_amount_,
      },
      {ultraviolet_tint, 0, desaturation, 0}
    );

    context.renderer.setViewer(
      player.position(),
      is_in_play && !player.isHidden() && context.world.map().width() > 0
    );
  }

  void Director::draw(GameContextDto &context) const {
    configurePostProcess(context);
    currentState().draw(*this, context);
  }
} // namespace wheel_of_fear
