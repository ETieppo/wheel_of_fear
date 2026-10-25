#pragma once

#include "core/render/Renderer.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/world/LedGradientProps.hpp"
#include "mods/world/RoomLedStrip.hpp"
#include "mods/world/RoomLightingPlanProps.hpp"
#include "mods/world/RoomPowerProps.hpp"
#include "mods/world/TileMapModel.hpp"
#include <cstddef>
#include <vector>

namespace wheel_of_fear {
  class RoomPowerGrid {
    public:
      [[nodiscard]]
      bool isPowered(std::size_t room_index) const;

      [[nodiscard]]
      bool isFullyLit(std::size_t room_index) const;

      [[nodiscard]]
      float litAmount(std::size_t room_index) const;

      void clear() {
        rooms_.clear();
        strips_.clear();
        lighting_plans_.clear();
      }

      void advance(float delta_seconds);

      void addRoom(
        LedGradientProps      gradient,
        RoomLedStrip          strip,
        float                 conduit_offset,
        RoomLightingPlanProps lighting_plan
      );

      void power(
        std::size_t room_index,
        bool        is_powered
      );

      void emitLights(
        const TileMapModel &map,
        LightingSystem     &lighting
      ) const;

      void emitRoomLights(
        LightingSystem &lighting,
        std::size_t     room_index
      ) const;

      void draw(
        const TileMapModel &map,
        Renderer           &renderer
      ) const;

      void paintRoomGlow(Renderer &renderer) const;

      [[nodiscard]]
      Vec2Props conduitPoint(std::size_t room_index) const;

      [[nodiscard]]
      float stripLength(std::size_t room_index) const;

      [[nodiscard]]
      ColorProps ledColorAt(
        std::size_t room_index,
        float       distance_along
      ) const;

    private:
      static constexpr float fill_light_intensity     = 1.2F;
      static constexpr float glow_fade_distance       = 48;
      static constexpr float glow_white_mix           = 0.35F;
      static constexpr float strip_power_up_seconds   = 3.2F;
      static constexpr float strip_power_down_seconds = 1.4F;
      static constexpr float strip_width              = 2.5F;
      static constexpr float doorway_spill_radius     = 170;
      static constexpr float doorway_spill_intensity  = 4.5F;
      static constexpr float doorway_freezing_radius  = 100;
      static constexpr float doorway_spill_setback    = RenderConfig::tile_size;

      [[nodiscard]]
      static float distanceFromConduit(
        const RoomPowerProps &power,
        float                 perimeter,
        float                 distance_along
      );

      [[nodiscard]]
      static float glowLevelAt(
        const RoomPowerProps &power,
        float                 perimeter,
        float                 distance_along
      );

      [[nodiscard]]
      static bool isStripLitAt(
        const RoomPowerProps &power,
        float                 perimeter,
        float                 distance_along
      );

      void emitDoorwaySpill(
        const TileMapModel &map,
        LightingSystem     &lighting,
        const DoorwayProps &doorway
      ) const;

      void drawPiece(
        Renderer                    &renderer,
        std::size_t                  room_index,
        const RoomLedPlacementProps &piece
      ) const;

      std::vector<RoomPowerProps>        rooms_;
      std::vector<RoomLedStrip>          strips_;
      std::vector<RoomLightingPlanProps> lighting_plans_;
  };
} // namespace wheel_of_fear
