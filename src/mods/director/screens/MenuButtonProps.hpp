#pragma once

#include "core/input/enums/InputActionEnum.hpp"
#include "core/math/RectProps.hpp"
#include "core/text/LocalizedTextProps.hpp"

namespace wheel_of_fear {
  struct MenuButtonProps {
      RectProps          bounds;
      InputActionEnum    action = InputActionEnum::confirm;
      LocalizedTextProps text;
  };
} // namespace wheel_of_fear
