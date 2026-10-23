#pragma once

#include "core/render/AtlasRegionProps.hpp"

namespace wheel_of_fear {
  struct PlayerPoseDto {
      AtlasRegionProps body {};
      AtlasRegionProps glow {};
  };
} // namespace wheel_of_fear
