#pragma once

#include "game/dtos/GameContextDto.hpp"
#include "mods/tracker/GhostEchoProps.hpp"
#include "mods/tracker/SignalProps.hpp"
#include <vector>

namespace wheel_of_fear {
  class TrackerSignalCollector {
    public:
      TrackerSignalCollector();

      [[nodiscard]]
      const std::vector<SignalProps> &signals() const {
        return signals_;
      }

      void collect(
        const GameContextDto              &context,
        const std::vector<GhostEchoProps> &ghosts
      );

    private:
      void addSignal(const SignalProps &signal);

      void collectGenerators(const GameContextDto &context);

      void collectFabricators(const GameContextDto &context);

      void collectExit(const GameContextDto &context);

      void collectDisguisedMobs(const GameContextDto &context);

      void collectGhosts(const std::vector<GhostEchoProps> &ghosts);

      std::vector<SignalProps> signals_;
  };
} // namespace wheel_of_fear
