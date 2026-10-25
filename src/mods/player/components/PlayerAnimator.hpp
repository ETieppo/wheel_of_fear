#pragma once

#include "mods/art/SpriteSheetProps.hpp"
#include "mods/player/dtos/PlayerPoseDto.hpp"
#include "mods/player/enums/PlayerActivityEnum.hpp"
#include <cstddef>

namespace wheel_of_fear {
  class PlayerAnimator final {
    public:
      PlayerAnimator() = delete;

      static PlayerPoseDto currentPose(
        const SpriteSheetProps &sprites,
        PlayerActivityEnum      activity,
        bool                    is_moving,
        float                   stride_distance,
        float                   real_seconds
      );

    private:
      static constexpr float stride_length = 30;

      static PlayerPoseDto pick(
        const SpriteFrames &bodies,
        std::size_t         frame
      );
  };
} // namespace wheel_of_fear
