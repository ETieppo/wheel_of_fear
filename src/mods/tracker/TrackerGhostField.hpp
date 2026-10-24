#pragma once

#include "game/dtos/GameContextDto.hpp"
#include "mods/tracker/GhostEchoProps.hpp"
#include <vector>

namespace wheel_of_fear {
  class TrackerGhostField {
    public:
      [[nodiscard]]
      const std::vector<GhostEchoProps> &ghosts() const {
        return ghosts_;
      }

      void update(GameContextDto &context);

    private:
      void driftAndExpire(float delta_seconds);
      void maybeSpawnGhost(
        GameContextDto &context,
        float           delta_seconds
      );

      std::vector<GhostEchoProps> ghosts_;
  };
} // namespace wheel_of_fear
