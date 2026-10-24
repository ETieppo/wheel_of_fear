#pragma once

#include <string_view>

#include "core/input/enums/InputActionEnum.hpp"

namespace wheel_of_fear {
  class KeyLabelCatalog final {
    public:
      KeyLabelCatalog() = delete;

      static std::string_view labelFor(
        InputActionEnum action
      );
  };
} // namespace wheel_of_fear
