#pragma once

#include "core/math/RandomGenerator.hpp"
#include "core/math/Vec2Props.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/world/LockerEntity.hpp"
#include "mods/world/MazeShifter.hpp"
#include "mods/world/NavigationService.hpp"
#include "mods/world/RoomPowerGrid.hpp"
#include "mods/world/ScorchMarkProps.hpp"
#include "mods/world/TileMapModel.hpp"
#include "mods/world/scenery/Scenery.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace wheel_of_fear {
  class WorldSystem {
    public:
      WorldSystem();

      void generatePhase(
        std::uint32_t phase,
        std::uint64_t seed
      );

      void update(GameContextDto &context);

      void draw(GameContextDto &context) const;

      TileMapModel &map() {
        return map_;
      }

      [[nodiscard]]
      const TileMapModel &map() const {
        return map_;
      }

      NavigationService &navigation() {
        return navigation_;
      }

      RandomGenerator &random() {
        return random_;
      }

      MazeShifter &mazeShifter() {
        return maze_shifter_;
      }

      [[nodiscard]]
      const RoomPowerGrid &power() const {
        return power_;
      }

      [[nodiscard]]
      std::uint32_t phase() const {
        return phase_;
      }

      [[nodiscard]]
      float elapsedSeconds() const {
        return elapsed_seconds_;
      }

      [[nodiscard]]
      float cameraShake() const {
        return camera_shake_;
      }

      [[nodiscard]]
      Vec2Props playerStart() const {
        return player_start_;
      }

      [[nodiscard]]
      Vec2Props exitPosition() const {
        return exit_position_;
      }

      [[nodiscard]]
      std::size_t startRoomIndex() const {
        return start_room_index_;
      }

      [[nodiscard]]
      std::size_t exitRoomIndex() const {
        return exit_room_index_;
      }

      std::vector<std::unique_ptr<Scenery>> &scenery() {
        return scenery_;
      }

      [[nodiscard]]
      const std::vector<std::unique_ptr<Scenery>> &scenery() const {
        return scenery_;
      }

      std::vector<LockerEntity> &lockers() {
        return lockers_;
      }

      [[nodiscard]]
      const std::vector<LockerEntity> &lockers() const {
        return lockers_;
      }

      [[nodiscard]]
      const std::vector<Vec2Props> &vents() const {
        return vents_;
      }

      [[nodiscard]]
      const std::vector<ScorchMarkProps> &scorchMarks() const {
        return scorch_marks_;
      }

      void addScenery(std::unique_ptr<Scenery> item) {
        scenery_.push_back(std::move(item));
      }

      void addLocker(const LockerEntity &locker) {
        lockers_.push_back(locker);
      }

      void addVent(Vec2Props vent) {
        vents_.push_back(vent);
      }

      void addScorchMark(Vec2Props position);

      void powerRoom(
        std::size_t room_index,
        bool        is_powered
      ) {
        power_.power(room_index, is_powered);
      }

      [[nodiscard]]
      bool isRoomPowered(std::size_t room_index) const {
        return power_.isPowered(room_index);
      }

      [[nodiscard]]
      bool isRoomFullyLit(std::size_t room_index) const {
        return power_.isFullyLit(room_index);
      }

      [[nodiscard]]
      bool isBurningLightAt(Vec2Props point) const;

      [[nodiscard]]
      bool isHazardAt(Vec2Props point) const;

      [[nodiscard]]
      bool isNearLandmark(
        int x,
        int y
      ) const;

      std::size_t randomPatrolRoom(
        std::size_t                from_room,
        std::optional<std::size_t> toward_room
      );

      [[nodiscard]]
      Vec2Props conduitPoint(std::size_t room_index) const;

      void shake(float amount);

    private:
      static constexpr float navigation_refresh_seconds = 0.25F;
      static constexpr float door_shift_noise_radius    = 360;
      static constexpr float door_shift_volume          = 0.55F;

      [[nodiscard]]
      LedGradientProps pickLedGradient();
      void             chooseLandmarks();

      void shiftMaze(
        GameContextDto            &context,
        std::span<const Vec2Props> occupants
      );

      [[nodiscard]]
      static std::vector<Vec2Props>
      collectOccupants(const GameContextDto &context);

      TileMapModel                          map_;
      NavigationService                     navigation_;
      RandomGenerator                       random_ {0xE6E6};
      float                                 elapsed_seconds_ = 0;
      float                                 camera_shake_    = 0;
      RoomPowerGrid                         power_;
      float                                 navigation_timer_ = 0;
      MazeShifter                           maze_shifter_;
      Vec2Props                             player_start_ {};
      Vec2Props                             exit_position_ {};
      std::uint32_t                         phase_            = 1;
      std::size_t                           start_room_index_ = 0;
      std::vector<std::unique_ptr<Scenery>> scenery_;
      std::vector<LockerEntity>             lockers_;
      std::vector<Vec2Props>                vents_;
      std::vector<ScorchMarkProps>          scorch_marks_;
      std::size_t                           exit_room_index_ = 0;
  };
} // namespace wheel_of_fear
