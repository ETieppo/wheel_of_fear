#pragma once

#include <filesystem>
#include <string>

#include <SDL3/SDL_loadso.h>

namespace wheel_of_fear {
  class SharedLibrary {
    public:
      explicit SharedLibrary(
        const std::filesystem::path &path
      );
      ~SharedLibrary();
      SharedLibrary(
        const SharedLibrary &
      ) = delete;
      SharedLibrary &operator =(
        const SharedLibrary &
      ) = delete;

      [[nodiscard]]
      SDL_FunctionPointer function(
        const std::string &name
      ) const;

    private:
      SDL_SharedObject* handle_ = nullptr;
  };
} // namespace wheel_of_fear
