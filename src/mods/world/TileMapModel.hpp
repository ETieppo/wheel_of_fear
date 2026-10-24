#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

#include "core/entity/Collider.hpp"
#include "core/math/RandomGenerator.hpp"
#include "core/math/Vec2Props.hpp"
#include "mods/world/DoorwayProps.hpp"
#include "mods/world/RoomProps.hpp"
#include "mods/world/TileCoordinateProps.hpp"
#include "mods/world/TileFluidLayer.hpp"

namespace wheel_of_fear {
  class TileMapModel final: public Collider {
    public:
      TileMapModel()
          : fluids_(*this) {
      }

      TileMapModel(
        const TileMapModel &
      ) = delete;
      TileMapModel &operator =(
        const TileMapModel &
      ) = delete;

      static TileCoordinateProps tileAt(
        Vec2Props point
      );
      static Vec2Props tileCenter(
        int x,
        int y
      );

      [[nodiscard]]
      int width() const {
        return width_;
      }

      [[nodiscard]]
      int height() const {
        return height_;
      }

      [[nodiscard]]
      std::uint64_t version() const {
        return version_;
      }

      [[nodiscard]]
      std::span<const std::uint8_t> walls() const {
        return walls_;
      }

      [[nodiscard]]
      const std::vector<RoomProps> &rooms() const {
        return rooms_;
      }

      [[nodiscard]]
      std::vector<DoorwayProps> &doorways() {
        return doorways_;
      }

      [[nodiscard]]
      const std::vector<DoorwayProps> &doorways() const {
        return doorways_;
      }

      [[nodiscard]]
      TileFluidLayer &fluids() {
        return fluids_;
      }

      [[nodiscard]]
      const TileFluidLayer &fluids() const {
        return fluids_;
      }

      void markChanged() {
        version_ += 1;
      }

      void reset(
        int width,
        int height
      );
      void carveFloor(
        int         x,
        int         y,
        std::size_t room_index
      );
      void raiseWall(
        int x,
        int y
      );
      void sealTile(
        int x,
        int y
      );
      void setRooms(
        std::vector<RoomProps> rooms
      );
      void setDoorwayTiles(
        const DoorwayProps &doorway,
        bool                is_passable
      );
      [[nodiscard]]
      bool isNearDoorway(
        int x,
        int y,
        int margin
      ) const;
      [[nodiscard]]
      bool isAreaFree(
        int x,
        int y,
        int area_width,
        int area_height
      ) const;
      bool reserveProp(
        int x,
        int y,
        int area_width,
        int area_height
      );
      void releaseProp(
        int x,
        int y,
        int area_width,
        int area_height
      );
      [[nodiscard]]
      std::optional<TileCoordinateProps> nearestOpenTile(
        TileCoordinateProps origin
      ) const;
      [[nodiscard]]
      Vec2Props nearestFloor(
        Vec2Props point
      ) const;
      [[nodiscard]]
      bool contains(
        int x,
        int y
      ) const;
      [[nodiscard]]
      std::size_t cellIndex(
        int x,
        int y
      ) const;
      [[nodiscard]]
      bool isWall(
        int x,
        int y
      ) const;
      [[nodiscard]]
      bool isStructural(
        int x,
        int y
      ) const;
      [[nodiscard]]
      bool isBlocked(
        int x,
        int y
      ) const;
      [[nodiscard]]
      bool isSolidAt(
        Vec2Props point
      ) const;
      [[nodiscard]]
      bool isWallAt(
        Vec2Props point
      ) const;
      bool dissolveWall(
        int x,
        int y
      );
      [[nodiscard]]
      Vec2Props slide(
        Vec2Props position,
        Vec2Props displacement,
        float     half_size
      ) const override;
      [[nodiscard]]
      bool hasLineOfSight(
        Vec2Props from,
        Vec2Props to
      ) const;
      [[nodiscard]]
      Vec2Props randomFloorInRoom(
        RandomGenerator &random,
        const RoomProps &room
      ) const;
      [[nodiscard]]
      std::optional<std::size_t> roomIndexAtTile(
        int x,
        int y
      ) const;
      [[nodiscard]]
      std::optional<std::size_t> roomIndexAt(
        Vec2Props point
      ) const;
      [[nodiscard]]
      int roomGridDistance(
        std::size_t first_room,
        std::size_t second_room
      ) const;

    private:
      [[nodiscard]]
      bool isInterior(
        int x,
        int y
      ) const;
      [[nodiscard]]
      bool collidesWithBox(
        Vec2Props center,
        float     half_size
      ) const;

      int                       width_  = 0;
      int                       height_ = 0;
      std::vector<std::uint8_t> walls_;
      std::vector<std::uint8_t> structural_;
      std::vector<std::uint8_t> blocked_;
      std::vector<std::int16_t> room_indices_;
      std::vector<RoomProps>    rooms_;
      std::vector<DoorwayProps> doorways_;
      TileFluidLayer            fluids_;
      std::uint64_t             version_ = 1;
  };
} // namespace wheel_of_fear
