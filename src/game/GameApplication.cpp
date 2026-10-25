#include "game/GameApplication.hpp"
#include <SDL3/SDL_events.h>
#include <algorithm>
#include <utility>

namespace wheel_of_fear {
  GameApplication::GameApplication(
    Window                              &window,
    std::optional<std::filesystem::path> progress_file_path
  )
      : window_(window)
      , gpu_context_(window_)
      , gpu_renderer_(gpu_context_)
      , simulation_(std::move(progress_file_path)) {
  }

  bool GameApplication::runFrame() {
    if(!pollEvents()) return false;

    simulation_.step(std::min(frame_timer_.lap(), maximum_frame_seconds));
    gpu_renderer_.render(simulation_.renderer());

    return true;
  }

  GameSnapshotProps GameApplication::snapshot() const {
    return simulation_.snapshot();
  }

  void GameApplication::resume(const GameSnapshotProps &snapshot) {
    simulation_.resume(snapshot);
  }

  bool GameApplication::pollEvents() {
    InputController &input = simulation_.input();
    input.beginFrame();
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
      input.handleEvent(event);
      if(event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
        gpu_context_.resize(window_.pixelWidth(), window_.pixelHeight());
    }

    input.setWindowSize(window_.logicalSize());
    return !input.isQuitRequested();
  }
} // namespace wheel_of_fear
