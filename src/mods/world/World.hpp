#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include "core/math/RandomGenerator.hpp"
#include "core/math/Vec2.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/world/Locker.hpp"
#include "mods/world/MazeShifter.hpp"
#include "mods/world/Navigation.hpp"
#include "mods/world/RoomPowerGrid.hpp"
#include "mods/world/ScorchMarkProps.hpp"
#include "mods/world/TileMap.hpp"
#include "mods/world/scenery/Scenery.hpp"

namespace wheel_of_fear {
  class World {
    public:
      World();

      void generatePhase(
        std::uint32_t phase,
        std::uint64_t seed
      );
      void update(
        GameContextDto &context
      );
      void draw(
        GameContextDto &context
      ) const;

      TileMap &map() {
        return map_;
      }

      const TileMap &map() const {
        return map_;
      }

      Navigation &navigation() {
        return navigation_;
      }

      RandomGenerator &random() {
        return random_;
      }

      MazeShifter &mazeShifter() {
        return maze_shifter_;
      }

      const RoomPowerGrid &power() const {
        return power_;
      }

      std::uint32_t phase() const {
        return phase_;
      }

      float elapsedSeconds() const {
        return elapsed_seconds_;
      }

      float cameraShake() const {
        return camera_shake_;
      }

      Vec2 playerStart() const {
        return player_start_;
      }

      Vec2 exitPosition() const {
        return exit_position_;
      }

      std::size_t startRoomIndex() const {
        return start_room_index_;
      }

      std::size_t exitRoomIndex() const {
        return exit_room_index_;
      }

      std::vector<std::unique_ptr<Scenery>> &scenery() {
        return scenery_;
      }

      const std::vector<std::unique_ptr<Scenery>> &scenery() const {
        return scenery_;
      }

      std::vector<Locker> &lockers() {
        return lockers_;
      }

      const std::vector<Locker> &lockers() const {
        return lockers_;
      }

      const std::vector<Vec2> &vents() const {
        return vents_;
      }

      const std::vector<ScorchMarkProps> &scorchMarks() const {
        return scorch_marks_;
      }

      void addScenery(
        std::unique_ptr<Scenery> item
      ) {
        scenery_.push_back(std::move(item));
      }

      void addLocker(
        const Locker &locker
      ) {
        lockers_.push_back(locker);
      }

      void addVent(
        Vec2 vent
      ) {
        vents_.push_back(vent);
      }

      void addScorchMark(
        Vec2 position
      );

      void powerRoom(
        std::size_t room_index,
        bool        is_powered
      ) {
        power_.power(room_index, is_powered);
      }

      bool isRoomPowered(
        std::size_t room_index
      ) const {
        return power_.isPowered(room_index);
      }

      bool isRoomFullyLit(
        std::size_t room_index
      ) const {
        return power_.isFullyLit(room_index);
      }

      bool isBurningLightAt(
        Vec2 point
      ) const;
      bool isHazardAt(
        Vec2 point
      ) const;
      bool isNearLandmark(
        int x,
        int y
      ) const;
      std::size_t randomPatrolRoom(
        std::size_t                from_room,
        std::optional<std::size_t> toward_room
      );
      Vec2 conduitPoint(
        std::size_t room_index
      ) const;
      void shake(
        float amount
      );

    private:
      static constexpr float navigation_refresh_seconds = 0.25F;
      static constexpr float door_shift_noise_radius    = 360;
      static constexpr float door_shift_volume          = 0.55F;

      void chooseLandmarks();
      void shiftMaze(
        GameContextDto       &context,
        std::span<const Vec2> occupants
      );
      std::vector<Vec2> collectOccupants(
        const GameContextDto &context
      ) const;

      TileMap                               map_;
      Navigation                            navigation_;
      RandomGenerator                       random_ {0xE6E6};
      std::uint32_t                         phase_           = 1;
      float                                 elapsed_seconds_ = 0;
      float                                 camera_shake_    = 0;
      RoomPowerGrid                         power_;
      std::vector<std::unique_ptr<Scenery>> scenery_;
      std::vector<Locker>                   lockers_;
      std::vector<Vec2>                     vents_;
      std::vector<ScorchMarkProps>          scorch_marks_;
      float                                 navigation_timer_ = 0;
      MazeShifter                           maze_shifter_;
      Vec2                                  player_start_ {};
      std::size_t                           start_room_index_ = 0;
      std::size_t                           exit_room_index_  = 0;
      Vec2                                  exit_position_ {};
  };
} // namespace wheel_of_fear
