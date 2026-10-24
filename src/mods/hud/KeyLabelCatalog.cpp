#include "mods/hud/KeyLabelCatalog.hpp"

namespace wheel_of_fear {
  std::string_view KeyLabelCatalog::labelFor(InputActionEnum action) {
    switch(action) {
      case InputActionEnum::interact:          return "E";
      case InputActionEnum::hold_breath:       return "SPACE";
      case InputActionEnum::throw_lure:        return "RMB";
      case InputActionEnum::throw_grenade:     return "G";
      case InputActionEnum::toggle_tracker:    return "TAB";
      case InputActionEnum::switch_light_mode: return "F";
      case InputActionEnum::hug:               return "LMB";
      case InputActionEnum::run:               return "SHIFT";
      case InputActionEnum::confirm:           return "ENTER";
      case InputActionEnum::back:              return "ESC";
      case InputActionEnum::restart:           return "R";
      case InputActionEnum::toggle_mute:       return "M";
      case InputActionEnum::move_up:           return "W";
      case InputActionEnum::move_down:         return "S";
      case InputActionEnum::move_left:         return "A";
      case InputActionEnum::move_right:        return "D";
    }

    return "";
  }
} // namespace wheel_of_fear
