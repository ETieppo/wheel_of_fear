#include "game/Game.hpp"

#include <algorithm>

#include <SDL3/SDL_events.h>

namespace wheel_of_fear {
  Game::Game(Window &window)
      : window_(window)
      , gpu_context_(window_)
      , gpu_renderer_(gpu_context_)
      , art_(renderer_)
      , context_ {
          clock_,
          input_,
          renderer_,
          sound_player_,
          art_,
          world_,
          lighting_,
          player_,
          mobs_,
          generators_,
          effects_,
          tracker_,
          hud_,
          noise_,
          throwables_,
          distractions_,
          camera_
        } {
    input_.setWindowSize(window_.logicalSize());
    director_.initialize(context_);
  }

  bool Game::runFrame() {
    if(!pollEvents()) return false;
    clock_.advance(std::min(frame_timer_.lap(), maximum_frame_seconds));
    tick();
    draw();
    gpu_renderer_.render(renderer_);
    return true;
  }

  GameSnapshotProps Game::snapshot() const {
    return GameSnapshotProps {director_.snapshot()};
  }

  void Game::resume(const GameSnapshotProps &snapshot) {
    director_.resume(context_, snapshot.director);
  }

  bool Game::pollEvents() {
    input_.beginFrame();
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      input_.handleEvent(event);
      if(event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
        gpu_context_.resize(window_.pixelWidth(), window_.pixelHeight());
    }
    input_.setWindowSize(window_.logicalSize());
    return !input_.isQuitRequested();
  }

  void Game::tick() {
    lighting_.beginFrame();
    hud_.beginFrame(clock_.realDeltaSeconds());
    director_.update(context_);
    noise_.update(clock_.deltaSeconds());
    world_.update(context_);
    player_.update(context_);
    camera_.update(context_);
    tracker_.update(context_);
    throwables_.update(context_);
    generators_.update(context_);
    distractions_.update(context_);
    mobs_.update(context_);
    effects_.update(context_);
    sound_player_.releaseFinished();
  }

  void Game::draw() {
    renderer_.beginFrame(camera_.viewCenter(), clock_.realSeconds());
    world_.draw(context_);
    generators_.draw(context_);
    throwables_.draw(context_);
    mobs_.draw(context_);
    player_.draw(context_);
    effects_.draw(context_);
    lighting_.submitToRenderer(renderer_);
    hud_.draw(context_);
    tracker_.draw(context_);
    director_.draw(context_);
  }
} // namespace wheel_of_fear
