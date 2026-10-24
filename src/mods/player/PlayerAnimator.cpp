#include "mods/player/PlayerAnimator.hpp"

#include <algorithm>

namespace wheel_of_fear {
  namespace {
    std::size_t frameAt(
      float value
    ) {
      return static_cast<std::size_t>(std::max(value, 0.0F));
    }
  } // namespace

  PlayerPoseDto PlayerAnimator::pick(
    const SpriteFrames &bodies,
    const SpriteFrames &glows,
    std::size_t         frame
  ) {
    const std::size_t index = frame % bodies.size();
    return PlayerPoseDto {bodies[index], glows[index % glows.size()]};
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
        return pick(
          sprites.player_hug,
          sprites.player_hug_glow,
          frameAt(real_seconds * 12)
        );
      case PlayerActivityEnum::repairing:
        return pick(
          sprites.player_repair,
          sprites.player_repair_glow,
          frameAt(real_seconds * 9)
        );
      default: break;
    }
    if(is_moving) {
      const float frame = stride_distance /
                          stride_length *
                          static_cast<float>(sprites.player_walk.size());
      return pick(
        sprites.player_walk,
        sprites.player_walk_glow,
        frameAt(frame)
      );
    }
    return pick(
      sprites.player_idle,
      sprites.player_idle_glow,
      frameAt(real_seconds * 7)
    );
  }
} // namespace wheel_of_fear
