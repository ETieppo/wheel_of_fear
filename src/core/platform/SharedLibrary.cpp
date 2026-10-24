#include "core/platform/SharedLibrary.hpp"

#include <stdexcept>

#include <SDL3/SDL_error.h>

namespace wheel_of_fear {
  SharedLibrary::SharedLibrary(
    const std::filesystem::path &path
  )
      : handle_(SDL_LoadObject(path.string().c_str())) {
    if(handle_ == nullptr)
      throw std::runtime_error(
        "unable to load library " + path.string() + ": " + SDL_GetError()
      );
  }

  SharedLibrary::~SharedLibrary() {
    SDL_UnloadObject(handle_);
  }

  SDL_FunctionPointer SharedLibrary::function(
    const std::string &name
  ) const {
    const SDL_FunctionPointer pointer = SDL_LoadFunction(handle_, name.c_str());
    if(pointer == nullptr)
      throw std::runtime_error(
        "missing library function " + name + ": " + SDL_GetError()
      );
    return pointer;
  }
} // namespace wheel_of_fear
