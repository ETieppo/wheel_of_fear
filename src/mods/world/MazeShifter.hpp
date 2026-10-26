#pragma once

#include "core/math/RandomGenerator.hpp"
#include "core/math/Vec2Props.hpp"
#include "mods/world/TileMapModel.hpp"
#include <array>
#include <cstddef>
#include <optional>
#include <span>
#include <vector>

namespace wheel_of_fear {
  class MazeShifter {
    public:
      void reset();

      static void animateDoorways(
        TileMapModel              &map,
        float                      delta_seconds,
        std::span<const Vec2Props> occupants
      );

      static void openDoorwaysNear(
        TileMapModel &map,
        Vec2Props     position
      );

      bool advance(
        TileMapModel              &map,
        RandomGenerator           &random,
        std::uint32_t              phase,
        float                      delta_seconds,
        Vec2Props                  view_center,
        std::optional<std::size_t> focus_room,
        std::span<const Vec2Props> occupants
      );

      [[nodiscard]]
      float timerSeconds() const {
        return timer_seconds_;
      }

      void setTimerSeconds(float seconds) {
        timer_seconds_ = seconds;
      }

      [[nodiscard]]
      const std::array<
        std::optional<std::size_t>,
        2
      > &movedDoorwayIndices() const {
        return moved_doorway_indices_;
      }

      static bool isInsideView(
        Vec2Props point,
        Vec2Props view_center
      );

    private:
      static constexpr float first_shift_delay_seconds = 20;

      static std::vector<std::size_t> collectCandidates(
        const TileMapModel        &map,
        std::size_t                focus_room,
        Vec2Props                  view_center,
        std::span<const Vec2Props> occupants,
        bool                       wants_open
      );

      static bool isOccupied(
        const DoorwayProps        &doorway,
        std::span<const Vec2Props> occupants
      );

      float timer_seconds_ = first_shift_delay_seconds;
      std::array<std::optional<std::size_t>, 2> moved_doorway_indices_ {};
  };
} // namespace wheel_of_fear
