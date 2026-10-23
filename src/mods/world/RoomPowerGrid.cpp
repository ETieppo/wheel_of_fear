#include "mods/world/RoomPowerGrid.hpp"

#include <algorithm>
#include <cmath>

#include "core/math/Scalar.hpp"
#include "core/render/RenderConfig.hpp"

namespace wheel_of_fear {
  void RoomPowerGrid::addRoom(
    Color color,
    float conduit_offset
  ) {
    rooms_.push_back(RoomPowerProps {color, false, 0, conduit_offset});
  }

  void RoomPowerGrid::power(
    std::size_t room_index,
    bool        is_powered
  ) {
    if(room_index >= rooms_.size()) return;
    rooms_[room_index].is_powered = is_powered;
  }

  bool RoomPowerGrid::isPowered(std::size_t room_index) const {
    return room_index < rooms_.size() && rooms_[room_index].is_powered;
  }

  bool RoomPowerGrid::isFullyLit(std::size_t room_index) const {
    return room_index < rooms_.size() && rooms_[room_index].isFullyLit();
  }

  float RoomPowerGrid::litAmount(std::size_t room_index) const {
    return room_index < rooms_.size() ? rooms_[room_index].lit_amount : 0.0F;
  }

  float RoomPowerGrid::perimeterOf(const RoomProps &room) {
    return 2 *
           (static_cast<float>(room.width) + static_cast<float>(room.height)) *
           RenderConfig::tile_size;
  }

  Vec2 RoomPowerGrid::perimeterPoint(
    const RoomProps &room,
    float            distance_along
  ) {
    const Vec2  minimum = room.worldMinimum().add(Vec2::of(1.5F, 1.5F));
    const Vec2  maximum = room.worldMaximum().sub(Vec2::of(1.5F, 1.5F));
    const float width   = maximum.x - minimum.x;
    const float height  = maximum.y - minimum.y;
    const float total   = 2 * (width + height);
    float       remaining =
      Scalar::floorModulo(distance_along / perimeterOf(room) * total, total);
    if(remaining < width) return Vec2::of(minimum.x + remaining, minimum.y);
    remaining -= width;
    if(remaining < height) return Vec2::of(maximum.x, minimum.y + remaining);
    remaining -= height;
    if(remaining < width) return Vec2::of(maximum.x - remaining, maximum.y);
    remaining -= width;
    return Vec2::of(minimum.x, maximum.y - remaining);
  }

  bool RoomPowerGrid::isStripLitAt(
    const RoomPowerProps &power,
    float                 perimeter,
    float                 distance_along
  ) {
    const float offset = std::abs(
      Scalar::floorModulo(
        distance_along - power.conduit_offset + perimeter * 1.5F, perimeter
      ) -
      perimeter *
      0.5F
    );
    return perimeter * 0.5F - offset <= power.lit_amount * perimeter * 0.5F;
  }

  void RoomPowerGrid::advance(float delta_seconds) {
    for(RoomPowerProps &power : rooms_) {
      const float rate = power.is_powered ? 1 / strip_power_up_seconds
                                          : -1 / strip_power_down_seconds;
      power.lit_amount =
        Scalar::clamp(power.lit_amount + rate * delta_seconds, 0, 1);
    }
  }

  Vec2 RoomPowerGrid::conduitPoint(
    const RoomProps &room,
    std::size_t      room_index
  ) const {
    return perimeterPoint(room, rooms_[room_index].conduit_offset);
  }

  void RoomPowerGrid::emitLights(
    const TileMap &map,
    Lighting      &lighting
  ) const {
    for(std::size_t room_index = 0; room_index < rooms_.size(); ++room_index) {
      const RoomPowerProps &power = rooms_[room_index];
      if(power.lit_amount <= 0) continue;
      const RoomProps &room      = map.rooms()[room_index];
      const float      perimeter = perimeterOf(room);
      const Vec2       center    = room.center();
      for(std::size_t anchor = 0; anchor < strip_light_count; ++anchor) {
        const float distance_along = power.conduit_offset +
                                     perimeter *
                                     (static_cast<float>(anchor) + 0.5F) /
                                     static_cast<float>(strip_light_count);
        if(!isStripLitAt(power, perimeter, distance_along)) continue;
        const Vec2 edge = perimeterPoint(room, distance_along);
        LightProps light;
        light.position =
          edge.add(center.sub(edge).normalized().scale(strip_light_inset));
        light.radius    = strip_light_radius;
        light.intensity = 0.85F;
        light.color     = power.color.mix(Color::white(), 0.55F);
        light.flicker   = 0.03F;
        lighting.add(light);
      }
    }
  }

  void RoomPowerGrid::draw(
    const TileMap &map,
    Renderer      &renderer
  ) const {
    for(std::size_t room_index = 0; room_index < rooms_.size(); ++room_index) {
      const RoomPowerProps &power   = rooms_[room_index];
      const RoomProps      &room    = map.rooms()[room_index];
      const Vec2            minimum = room.worldMinimum();
      const Vec2            maximum = room.worldMaximum();
      if(!renderer.isVisible(
           room.center(), std::max(maximum.x - minimum.x, maximum.y - minimum.y)
         ))
        continue;
      const float perimeter     = perimeterOf(room);
      const float head_distance = power.lit_amount * perimeter * 0.5F;
      for(float distance_along = 0; distance_along < perimeter;
          distance_along += strip_led_spacing) {
        const Vec2 point = perimeterPoint(room, distance_along);
        if(!renderer.isVisible(point, 8)) continue;
        if(map.isWallAt(point)) continue;
        if(!isStripLitAt(power, perimeter, distance_along)) {
          renderer.drawGlowRectangle(
            point, Vec2::of(1.5F, 1.5F), Color::hex(0x10141c).withAlpha(0.35F)
          );
          continue;
        }
        const float offset = std::abs(
          Scalar::floorModulo(
            distance_along - power.conduit_offset + perimeter * 1.5F, perimeter
          ) -
          perimeter *
          0.5F
        );
        const float head_glow =
          Scalar::clamp(
            1 - std::abs((perimeter * 0.5F - offset) - head_distance) / 24, 0, 1
          ) *
          (1 - power.lit_amount * power.lit_amount);
        renderer.drawGlowRectangle(
          point,
          Vec2::of(3, 3),
          power.color.mix(Color::white(), 0.3F + head_glow * 0.6F)
        );
        if(head_glow > 0.05F)
          renderer.drawGlowDisc(
            point, 10, Color::white().withAlpha(head_glow * 0.6F), 1.4F
          );
      }
      if(power.lit_amount > 0)
        renderer.drawGlowDisc(
          perimeterPoint(room, power.conduit_offset),
          7,
          power.color.withAlpha(0.8F),
          1.2F
        );
    }
  }
} // namespace wheel_of_fear
