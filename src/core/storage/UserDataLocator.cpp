#include "core/storage/UserDataLocator.hpp"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>

namespace wheel_of_fear {
  std::optional<std::filesystem::path>
  UserDataLocator::filePath(std::string_view file_name) {
    char* directory = SDL_GetPrefPath(organization_name, application_name);

    if(directory == nullptr) {
      SDL_Log("user data directory unavailable: %s", SDL_GetError());
      return std::nullopt;
    }

    const std::filesystem::path path
      = std::filesystem::path(directory) / file_name;
    SDL_free(directory);
    return path;
  }
} // namespace wheel_of_fear
