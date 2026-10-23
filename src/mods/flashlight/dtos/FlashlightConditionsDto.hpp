#pragma once

namespace wheel_of_fear {
  struct FlashlightConditionsDto {
      float delta_seconds       = 0;
      bool  is_held_in_mouth    = false;
      bool  is_tracker_open     = false;
      bool  is_holder_hidden    = false;
      bool  is_in_burning_light = false;
  };
} // namespace wheel_of_fear
