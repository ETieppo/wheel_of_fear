#include "core/storage/TextFileStorage.hpp"
#include <SDL3/SDL_log.h>
#include <fstream>
#include <iterator>
#include <system_error>
#include <utility>

namespace wheel_of_fear {
  TextFileStorage::TextFileStorage(std::filesystem::path path)
      : path_(std::move(path)) {
  }

  std::optional<std::string> TextFileStorage::read() const {
    std::ifstream stream(path_, std::ios::binary);
    if(!stream) return std::nullopt;

    return std::string {
      std::istreambuf_iterator<char>(stream),
      std::istreambuf_iterator<char>()
    };
  }

  bool TextFileStorage::write(std::string_view text) const {
    std::filesystem::path pending_path = path_;
    pending_path += ".pending";

    {
      std::ofstream stream(pending_path, std::ios::binary | std::ios::trunc);
      stream.write(text.data(), static_cast<std::streamsize>(text.size()));

      if(!stream) {
        SDL_Log("unable to write %s", pending_path.string().c_str());
        return false;
      }
    }

    std::error_code error;
    std::filesystem::rename(pending_path, path_, error);
    if(!error) return true;

    SDL_Log(
      "unable to replace %s: %s",
      path_.string().c_str(),
      error.message().c_str()
    );
    return false;
  }

  void TextFileStorage::remove() const {
    std::error_code error;
    std::filesystem::remove(path_, error);
  }
} // namespace wheel_of_fear
