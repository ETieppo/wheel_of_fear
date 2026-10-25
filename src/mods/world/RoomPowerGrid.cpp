#include "mods/world/RoomPowerGrid.hpp"
#include "core/math/ScalarMath.hpp"
#include <algorithm>
#include <cmath>
#include <utility>

namespace wheel_of_fear {
  void RoomPowerGrid::addRoom(
    LedGradientProps      gradient,
    RoomLedStrip          strip,
    float                 conduit_offset,
    RoomLightingPlanProps lighting_plan
  ) {
    rooms_.push_back(
      RoomPowerProps {
        .gradient       = gradient,
        .is_powered     = false,
        .lit_amount     = 0,
        .conduit_offset = conduit_offset,
      }
    );

    strips_.push_back(std::move(strip));
    lighting_plans_.push_back(std::move(lighting_plan));
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
    return distanceFromConduit(power, perimeter, distance_along)
      <= power.lit_amount
      * perimeter
      * 0.5F;
  }

  float RoomPowerGrid::glowLevelAt(
    const RoomPowerProps &power,
    float                 perimeter,
    float                 distance_along
  ) {
    if(power.lit_amount <= 0) return 0;
    const float head_distance = power.lit_amount * perimeter * 0.5F;
    const float behind_head
      = head_distance - distanceFromConduit(power, perimeter, distance_along);
    return ScalarMath::clamp(1 + (behind_head / glow_fade_distance), 0, 1);
  }

  void RoomPowerGrid::paintRoomGlow(Renderer &renderer) const {
    for(std::size_t room_index = 0; room_index < rooms_.size(); ++room_index) {
      const RoomPowerProps &power     = rooms_[room_index];
      const float           perimeter = strips_[room_index].length();

      for(
        const RoomGlowTileProps &tile : lighting_plans_[room_index].glow_tiles
      ) {
        const float level = glowLevelAt(power, perimeter, tile.distance_along);
        const ColorProps glow = ledColorAt(room_index, tile.distance_along)
                                  .mix(ColorProps::white(), glow_white_mix)
                                  .scaled(level);

        renderer.setRoomGlow(tile.x, tile.y, glow);
      }
    }
  }

  void RoomPowerGrid::advance(float delta_seconds) {
    for(RoomPowerProps &power : rooms_) {
      const float rate = power.is_powered ? 1 / strip_power_up_seconds
                                          : -1 / strip_power_down_seconds;

      power.lit_amount
        = ScalarMath::clamp(power.lit_amount + (rate * delta_seconds), 0, 1);
    }
  }

  Vec2Props RoomPowerGrid::conduitPoint(std::size_t room_index) const {
    return strips_[room_index].pointAt(rooms_[room_index].conduit_offset);
  }

  float RoomPowerGrid::stripLength(std::size_t room_index) const {
    return strips_[room_index].length();
  }

  ColorProps RoomPowerGrid::ledColorAt(
    std::size_t room_index,
    float       distance_along
  ) const {
    const RoomPowerProps &power = rooms_[room_index];
    const float           fraction_along
      = (distance_along - power.conduit_offset) / strips_[room_index].length();

    return power.gradient.colorAt(fraction_along);
  }

  void RoomPowerGrid::emitLights(
    const TileMapModel &map,
    LightingSystem     &lighting
  ) const {
    for(std::size_t room_index = 0; room_index < rooms_.size(); ++room_index)
      emitRoomLights(lighting, room_index);

    for(const DoorwayProps &doorway : map.doorways())
      emitDoorwaySpill(map, lighting, doorway);
  }

  void RoomPowerGrid::emitDoorwaySpill(
    const TileMapModel &map,
    LightingSystem     &lighting,
    const DoorwayProps &doorway
  ) const {
    if(doorway.room_a >= rooms_.size() || doorway.room_b >= rooms_.size())
      return;

    const std::size_t lit_room
      = litAmount(doorway.room_a) >= litAmount(doorway.room_b) ? doorway.room_a
                                                               : doorway.room_b;
    const float spill = litAmount(lit_room) * doorway.open_amount;
    if(spill <= 0) return;

    const Vec2Props center = doorway.center();
    const Vec2Props across
      = doorway.is_vertical ? Vec2Props::of(1, 0) : Vec2Props::of(0, 1);
    const float toward_lit_room
      = across.dot(map.rooms()[lit_room].center().sub(center)) >= 0 ? 1.0F
                                                                    : -1.0F;
    LightProps light;

    light.position
      = center.add(across.scale(doorway_spill_setback * toward_lit_room));
    light.reveals_area = true;
    light.radius       = doorway_spill_radius;
    light.intensity    = doorway_spill_intensity * spill;
    light.color
      = ledColorAt(lit_room, strips_[lit_room].nearestDistanceAlong(center))
          .mix(ColorProps::white(), 0.55F);
    light.flicker           = 0.03F;
    light.freezes_creatures = true;
    light.freezing_radius   = doorway_freezing_radius;
    lighting.add(light);
  }

  void RoomPowerGrid::emitRoomLights(
    LightingSystem &lighting,
    std::size_t     room_index
  ) const {
    const RoomPowerProps &power = rooms_[room_index];
    if(power.lit_amount <= 0) return;
    const float perimeter = strips_[room_index].length();

    for(
      const RoomFillLightProps &fill : lighting_plans_[room_index].fill_lights
    ) {
      if(!isStripLitAt(power, perimeter, fill.distance_along)) continue;

      LightProps light;
      light.position     = fill.position;
      light.radius       = fill.radius;
      light.intensity    = fill_light_intensity;
      light.color        = ledColorAt(room_index, fill.distance_along)
                             .mix(ColorProps::white(), 0.55F);
      light.flicker      = 0.03F;
      light.bounds       = lighting_plans_[room_index].fill_bounds;
      light.reveals_area = true;
      lighting.add(light);
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
      const float           reach
        = std::max(maximum.x - minimum.x, maximum.y - minimum.y);

      if(!renderer.isVisible(room.center(), reach)) continue;
      const RoomLedStrip &strip = strips_[room_index];

      for(const RoomLedPlacementProps &piece : strip.placements(map)) {
        if(piece.faces_open_passage || piece.is_covered_by_object) continue;
        if(!renderer.isVisible(piece.position, 8)) continue;
        drawPiece(renderer, room_index, piece);
      }

      if(power.lit_amount > 0)
        renderer.drawGlowDisc(
          strip.pointAt(power.conduit_offset),
          7,
          ledColorAt(room_index, power.conduit_offset).withAlpha(0.8F),
          1.2F
        );
    }
  }

  void RoomPowerGrid::drawPiece(
    Renderer                    &renderer,
    std::size_t                  room_index,
    const RoomLedPlacementProps &piece
  ) const {
    const RoomPowerProps &power     = rooms_[room_index];
    const float           perimeter = strips_[room_index].length();
    const float     middle_along    = piece.distance_along + (piece.span / 2);
    const Vec2Props center          = piece.position.add(
      piece.direction.scale((piece.span + strip_width) / 2)
    );
    const Vec2Props size
      = std::abs(piece.direction.x) > std::abs(piece.direction.y)
      ? Vec2Props::of(piece.span, strip_width)
      : Vec2Props::of(strip_width, piece.span);

    if(!isStripLitAt(power, perimeter, middle_along)) {
      renderer.drawGlowRectangle(
        center,
        size,
        ColorProps::hex(0x10141c).withAlpha(0.35F)
      );

      return;
    }

    const float head_distance = power.lit_amount * perimeter * 0.5F;
    const float lit_distance
      = distanceFromConduit(power, perimeter, middle_along);
    const float head_glow = ScalarMath::clamp(
                              1 - (std::abs(lit_distance - head_distance) / 24),
                              0,
                              1
                            )
      * (1 - (power.lit_amount * power.lit_amount));

    const ColorProps color
      = ledColorAt(room_index, middle_along)
          .mix(ColorProps::white(), 0.2F + (head_glow * 0.6F));

    renderer.drawGlowRectangle(center, size, color);
    if(head_glow > 0.05F)
      renderer.drawGlowDisc(
        center,
        10,
        ColorProps::white().withAlpha(head_glow * 0.6F),
        1.4F
      );
  }
} // namespace wheel_of_fear
