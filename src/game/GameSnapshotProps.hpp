#pragma once

#include "core/text/enums/LanguageEnum.hpp"
#include "mods/director/DirectorSnapshotProps.hpp"

namespace wheel_of_fear {
  struct GameSnapshotProps {
      DirectorSnapshotProps director;
      LanguageEnum          language = LanguageEnum::english;
  };
} // namespace wheel_of_fear
