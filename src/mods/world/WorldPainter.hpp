#pragma once

#include "game/dtos/GameContextDto.hpp"
#include "mods/world/DoorwayProps.hpp"

namespace wheel_of_fear {
  class WorldSystem;
  class Renderer;
  struct SpriteSheetProps;

  class WorldPainter {
    public:
      WorldPainter(
        const WorldSystem &world,
        GameContextDto    &context
      );

      void paint() const;

    private:
      void paintFloorLiquids() const;
      void paintScorchMarks() const;
      void paintVents() const;
      void paintDoorway(
        const DoorwayProps &doorway
      ) const;
      void paintScenery() const;
      void paintLockers() const;

      const WorldSystem      &world_;
      Renderer               &renderer_;
      const SpriteSheetProps &sprites_;
      float                   real_seconds_;
  };
} // namespace wheel_of_fear
