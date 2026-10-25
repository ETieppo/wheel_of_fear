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
      static constexpr float soft_circle_radius  = 8;
      static constexpr float neon_film_opacity   = 0.45F;
      static constexpr float neon_glow_opacity   = 0.6F;
      static constexpr float neon_core_opacity   = 0.5F;
      static constexpr float neon_halo_opacity   = 0.12F;
      static constexpr float neon_core_white_mix = 0.65F;

      void paintAcidPools() const;
      void paintNeonSplashes() const;
      void paintScorchMarks() const;
      void paintVents() const;
      void paintDoorway(const DoorwayProps &doorway) const;
      void paintScenery() const;
      void paintLockers() const;

      const WorldSystem &world_;
      Renderer          &renderer_;

      const SpriteSheetProps &sprites_;
      float                   real_seconds_;
  };
} // namespace wheel_of_fear
