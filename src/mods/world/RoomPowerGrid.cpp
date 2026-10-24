#include "mods/world/RoomPowerGrid.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

#include "core/math/ScalarMath.hpp"

namespace wheel_of_fear {
  void RoomPowerGrid::addRoom(
    ColorProps   color,
    RoomLedStrip strip,
    float        conduit_offset
  ) {
    rooms_.push_back(
      RoomPowerProps {
        .color          = color,
        .is_powered     = false,
        .lit_amount     = 0,
        .conduit_offset = conduit_offset,
      }
    );

    strips_.push_back(std::move(strip));
  }

  void RoomPowerGrid::power(
    std::size_t room_index,
    bool        is_powered
  ) {
    if(room_index >= rooms_.size()) return;

    rooms_[room_index].is_powered = is_powered;
  }

  bool RoomPowerGrid::isPowered(
    std::size_t room_index
  ) const {
    return room_index < rooms_.size() && rooms_[room_index].is_powered;
  }

  bool RoomPowerGrid::isFullyLit(
    std::size_t room_index
  ) const {
    return room_index < rooms_.size() && rooms_[room_index].isFullyLit();
  }

  float RoomPowerGrid::litAmount(
    std::size_t room_index
  ) const {
    return room_index < rooms_.size() ? rooms_[room_index].lit_amount : 0.0F;
  }

  float RoomPowerGrid::distanceFromConduit(
    const RoomPowerProps &power,
    float                 perimeter,
    float                 distance_along
  ) {
    const float wrapped = ScalarMath::floorModulo(
      distance_along - power.conduit_offset + (perimeter * 1.5F),
      perimeter
    );

    return (perimeter * 0.5F) - std::abs(wrapped - (perimeter * 0.5F));
  }

  bool RoomPowerGrid::isStripLitAt(
    const RoomPowerProps &power,
    float                 perimeter,
    float                 distance_along
  ) {
    return distanceFromConduit(power, perimeter, distance_along) <=
           power.lit_amount *
           perimeter *
           0.5F;
  }

  void RoomPowerGrid::advance(
    float delta_seconds
  ) {
    for(RoomPowerProps &power : rooms_) {
      const float rate = power.is_powered ? 1 / strip_power_up_seconds
                                          : -1 / strip_power_down_seconds;

      power.lit_amount =
        ScalarMath::clamp(power.lit_amount + (rate * delta_seconds), 0, 1);
    }
  }

  Vec2Props RoomPowerGrid::conduitPoint(
    std::size_t room_index
  ) const {
    return strips_[room_index].pointAt(rooms_[room_index].conduit_offset);
  }

  void RoomPowerGrid::emitLights(
    LightingSystem &lighting
  ) const {
    for(std::size_t room_index = 0; room_index < rooms_.size(); ++room_index) {
      const RoomPowerProps &power = rooms_[room_index];
      if(power.lit_amount <= 0) continue;

      const RoomLedStrip &strip     = strips_[room_index];
      const float         perimeter = strip.length();

      for(std::size_t anchor = 0; anchor < strip_light_count; ++anchor) {
        const float distance_along =
          power.conduit_offset + (perimeter *
                                  (static_cast<float>(anchor) + 0.5F) /
                                  static_cast<float>(strip_light_count));
        if(!isStripLitAt(power, perimeter, distance_along)) continue;

        LightProps light;

        light.position =
          strip.pointAt(distance_along)
            .add(strip.inwardAt(distance_along).scale(strip_light_inset));

        light.radius    = strip_light_radius;
        light.intensity = 0.85F;
        light.color     = power.color.mix(ColorProps::white(), 0.55F);
        light.flicker   = 0.03F;
        lighting.add(light);
      }
    }
  }

  void RoomPowerGrid::draw(
    const TileMapModel &map,
    Renderer           &renderer
  ) const {
    for(std::size_t room_index = 0; room_index < rooms_.size(); ++room_index) {
      const RoomPowerProps &power   = rooms_[room_index];
      const RoomProps      &room    = map.rooms()[room_index];
      const Vec2Props       minimum = room.worldMinimum();
      const Vec2Props       maximum = room.worldMaximum();

      const float reach =
        std::max(maximum.x - minimum.x, maximum.y - minimum.y);
      if(!renderer.isVisible(room.center(), reach)) continue;

      const RoomLedStrip &strip     = strips_[room_index];
      const float         perimeter = strip.length();

      for(const RoomLedPlacementProps &led : strip.placements(map)) {
        if(led.faces_open_passage || led.is_covered_by_object) continue;
        if(!renderer.isVisible(led.position, 8)) continue;

        drawLed(renderer, power, perimeter, led);
      }

      if(power.lit_amount > 0)
        renderer.drawGlowDisc(
          strip.pointAt(power.conduit_offset),
          7,
          power.color.withAlpha(0.8F),
          1.2F
        );
    }
  }

  void RoomPowerGrid::drawLed(
    Renderer                    &renderer,
    const RoomPowerProps        &power,
    float                        perimeter,
    const RoomLedPlacementProps &led
  ) {
    if(!isStripLitAt(power, perimeter, led.distance_along)) {
      renderer.drawGlowRectangle(
        led.position,
        Vec2Props::of(1.5F, 1.5F),
        ColorProps::hex(0x10141c).withAlpha(0.35F)
      );

      return;
    }

    const float head_distance = power.lit_amount * perimeter * 0.5F;

    const float lit_distance =
      distanceFromConduit(power, perimeter, led.distance_along);

    const float head_glow = ScalarMath::clamp(
                              1 - (std::abs(lit_distance - head_distance) / 24),
                              0,
                              1
                            ) *
                            (1 - (power.lit_amount * power.lit_amount));

    renderer.drawGlowRectangle(
      led.position,
      Vec2Props::of(3, 3),
      power.color.mix(ColorProps::white(), 0.3F + (head_glow * 0.6F))
    );

    if(head_glow > 0.05F)
      renderer.drawGlowDisc(
        led.position,
        10,
        ColorProps::white().withAlpha(head_glow * 0.6F),
        1.4F
      );
  }
} // namespace wheel_of_fear
