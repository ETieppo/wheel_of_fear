#pragma once

#include "core/entity/Collider.hpp"
#include "core/math/RandomGenerator.hpp"
#include "core/math/Vec2.hpp"
#include "mods/world/DoorwayProps.hpp"
#include "mods/world/RoomProps.hpp"
#include "mods/world/TileCoordinateProps.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace wheel_of_fear {
  class TileMap final: public Collider {
    public:
      static constexpr int room_cell_width  = 30;
      static constexpr int room_cell_height = 20;
      static constexpr int free_border      = 3;

      void generate(
        RandomGenerator &random,
        int              grid_columns,
        int              grid_rows
      );

      int width() const {
        return width_;
      } // as

      int height() const {
        return height_;
      }

      std::uint64_t version() const {
        return version_;
      }

      std::span<const std::uint8_t> walls() const {
        return walls_;
      }

      std::span<const float> acidSeconds() const {
        return acid_seconds_;
      }

      std::span<const float> neonSeconds() const {
        return neon_seconds_;
      }

      const std::vector<RoomProps> &rooms() const {
        return rooms_;
      }

      std::vector<DoorwayProps> &doorways() {
        return doorways_;
      }

      const std::vector<DoorwayProps> &doorways() const {
        return doorways_;
      }

      void setDoorwayTiles(
        const DoorwayProps &doorway,
        bool                is_passable
      );
      bool isRoomGraphConnected() const;
      bool isNearDoorway(
        int x,
        int y,
        int margin
      ) const;
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
      bool isRoomOpenAreaConnected(const RoomProps &room) const;
      bool isEveryFloorConnected() const;
      Vec2 nearestFloor(Vec2 point) const;

      bool contains(
        int x,
        int y
      ) const;
      std::size_t cellIndex(
        int x,
        int y
      ) const;
      bool isWall(
        int x,
        int y
      ) const;
      bool isStructural(
        int x,
        int y
      ) const;
      bool isBlocked(
        int x,
        int y
      ) const;
      bool  isSolidAt(Vec2 point) const;
      bool  isWallAt(Vec2 point) const;
      float acidAt(Vec2 point) const;
      void  spillAcid(
        Vec2  point,
        float seconds
      );
      float neonAt(Vec2 point) const;
      float neonAtTile(
        int x,
        int y
      ) const;
      void spillNeon(
        int   x,
        int   y,
        float seconds
      );
      bool dissolveWall(
        int x,
        int y
      );
      void evaporate(float seconds);
      Vec2 slide(
        Vec2  position,
        Vec2  displacement,
        float half_size
      ) const override;
      bool hasLineOfSight(
        Vec2 from,
        Vec2 to
      ) const;
      Vec2 randomFloorInRoom(
        RandomGenerator &random,
        const RoomProps &room
      ) const;
      std::optional<std::size_t> roomIndexAtTile(
        int x,
        int y
      ) const;
      std::optional<std::size_t> roomIndexAt(Vec2 point) const;
      int                        roomGridDistance(
        std::size_t first_room,
        std::size_t second_room
      ) const;

      static
      TileCoordinateProps tileAt(Vec2 point);
      static
      Vec2 tileCenter(
        int x,
        int y
      );

    private:
      void carveRooms(RandomGenerator &random);
      void placeDoorways(RandomGenerator &random);
      void openSpanningDoorways(RandomGenerator &random);
      void furnishRoom(
        RandomGenerator &random,
        std::size_t      room_index
      );
      bool placeWallBlock(
        const RoomProps &room,
        int              x,
        int              y,
        int              block_width,
        int              block_height
      );
      void sealUnreachableFloor(const RoomProps &room);
      std::optional<TileCoordinateProps>
           nearestOpenTile(TileCoordinateProps origin) const;
      bool isInterior(
        int x,
        int y
      ) const;
      bool collidesWithBox(
        Vec2  center,
        float half_size
      ) const;

      void markChanged() {
        version_ += 1;
      }

      static
      bool isInClearing(
        const RoomProps &room,
        int              x,
        int              y
      );
      static
      bool isInsideFurnishableArea(
        const RoomProps &room,
        int              x,
        int              y
      );
      static
      int randomInteriorX(
        const RoomProps &room,
        RandomGenerator &random
      );
      static
      int randomInteriorY(
        const RoomProps &room,
        RandomGenerator &random
      );

      int                       width_        = 0;
      int                       height_       = 0;
      int                       grid_columns_ = 0;
      int                       grid_rows_    = 0;
      std::vector<std::uint8_t> walls_;
      std::vector<std::uint8_t> structural_;
      std::vector<std::uint8_t> blocked_;
      std::vector<float>        acid_seconds_;
      std::vector<float>        neon_seconds_;
      std::vector<std::int16_t> room_indices_;
      std::vector<RoomProps>    rooms_;
      std::vector<DoorwayProps> doorways_;
      std::uint64_t             version_ = 1;
  };
} // namespace wheel_of_fear
