#pragma once

#include "core/input/enums/InputActionEnum.hpp"
#include <string_view>

namespace wheel_of_fear {
  class KeyLabelCatalog final {
    public:
      KeyLabelCatalog() = delete;

      static std::string_view labelFor(InputActionEnum action);
  };
} // namespace wheel_of_fear
