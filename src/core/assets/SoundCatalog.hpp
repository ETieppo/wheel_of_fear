#pragma once

#include "core/audio/enums/MusicLayerEnum.hpp"
#include "core/audio/enums/SoundEnum.hpp"
#include <span>
#include <string_view>

namespace wheel_of_fear {
  class SoundCatalog final {
    public:
      SoundCatalog() = delete;

      static std::span<const std::string_view> filesFor(SoundEnum sound);
      static std::string_view                  musicFor(MusicLayerEnum layer);
  };
} // namespace wheel_of_fear
