#pragma once

#include <cstddef>
#include <vector>

#include "core/render/Renderer.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/world/RoomLedStrip.hpp"
#include "mods/world/RoomPowerProps.hpp"
#include "mods/world/TileMapModel.hpp"

namespace wheel_of_fear {
  class RoomPowerGrid {
    public:
      [[nodiscard]]
      bool isPowered(
        std::size_t room_index
      ) const;
      [[nodiscard]]
      bool isFullyLit(
        std::size_t room_index
      ) const;
      [[nodiscard]]
      float litAmount(
        std::size_t room_index
      ) const;

      void clear() {
        rooms_.clear();
        strips_.clear();
      }

      void advance(
        float delta_seconds
      );
      void addRoom(
        ColorProps   color,
        RoomLedStrip strip,
        float        conduit_offset
      );
      void power(
        std::size_t room_index,
        bool        is_powered
      );
      void emitLights(
        LightingSystem &lighting
      ) const;
      void draw(
        const TileMapModel &map,
        Renderer           &renderer
      ) const;
      [[nodiscard]]
      Vec2Props conduitPoint(
        std::size_t room_index
      ) const;

    private:
      static constexpr std::size_t strip_light_count        = 6;
      static constexpr float       strip_light_radius       = 175;
      static constexpr float       strip_light_inset        = 18;
      static constexpr float       strip_power_up_seconds   = 3.2F;
      static constexpr float       strip_power_down_seconds = 1.4F;

      [[nodiscard]]
      static float distanceFromConduit(
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
      static void drawLed(
        Renderer                    &renderer,
        const RoomPowerProps        &power,
        float                        perimeter,
        const RoomLedPlacementProps &led
      );

      std::vector<RoomPowerProps> rooms_;
      std::vector<RoomLedStrip>   strips_;
  };
} // namespace wheel_of_fear
