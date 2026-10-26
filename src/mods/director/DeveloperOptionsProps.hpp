#pragma once

#include "core/text/enums/LanguageEnum.hpp"
#include <optional>

namespace wheel_of_fear {
  struct DeveloperOptionsProps {
      bool                        is_god_mode = false;
      std::optional<LanguageEnum> language    = std::nullopt;
  };
} // namespace wheel_of_fear
