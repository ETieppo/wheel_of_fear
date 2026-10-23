#include <exception>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>

#include "core/platform/Window.hpp"
#include "game/GameRunner.hpp"

int main(
  int,
  char**
) {
  try {
    wheel_of_fear::Window window("Wheel of Fear", 1440, 810);
    const auto            runner = wheel_of_fear::GameRunner::create(window);
    while(runner->runFrame()) {}
  } catch(const std::exception &error) {
    SDL_Log("fatal: %s", error.what());
    return 1;
  }

  return 0;
}
