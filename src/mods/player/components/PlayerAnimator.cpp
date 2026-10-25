#include "mods/player/components/PlayerAnimator.hpp"
#include <algorithm>

namespace wheel_of_fear {
  namespace {
    std::size_t frameAt(float value) {
      return static_cast<std::size_t>(std::max(value, 0.0F));
    }
  } // namespace

  PlayerPoseDto PlayerAnimator::pick(
    const SpriteFrames &bodies,
    std::size_t         frame
  ) {
    return PlayerPoseDto {.body = bodies[frame % bodies.size()]};
  }

  PlayerPoseDto PlayerAnimator::currentPose(
    const SpriteSheetProps &sprites,
    PlayerActivityEnum      activity,
    bool                    is_moving,
    float                   stride_distance,
    float                   real_seconds
  ) {
    switch(activity) {
      case PlayerActivityEnum::hugging:
        return pick(sprites.player_hug, frameAt(real_seconds * 12));

      case PlayerActivityEnum::repairing:
        return pick(sprites.player_repair, frameAt(real_seconds * 9));

      default: break;
    }

    if(is_moving) {
      const float frame = stride_distance
        / stride_length
        * static_cast<float>(sprites.player_walk.size());

      return pick(sprites.player_walk, frameAt(frame));
    }

    return pick(sprites.player_idle, frameAt(real_seconds * 7));
  }
} // namespace wheel_of_fear
