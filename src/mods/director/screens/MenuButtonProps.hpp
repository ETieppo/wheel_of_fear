#pragma once

#include "core/input/enums/InputActionEnum.hpp"
#include "core/math/RectProps.hpp"
#include <string_view>

namespace wheel_of_fear {
  struct MenuButtonProps {
      RectProps        bounds;
      InputActionEnum  action = InputActionEnum::confirm;
      std::string_view text;
  };
} // namespace wheel_of_fear
