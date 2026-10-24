#include "mods/generators/GeneratorEntity.hpp"

#include <algorithm>
#include <cmath>
#include <format>

#include "core/audio/SoundPlayer.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/Renderer.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/effects/EffectSystem.hpp"
#include "mods/hud/HudSystem.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  GeneratorEntity::GeneratorEntity(
    Vec2Props               position,
    std::size_t             room_index,
    char                    label,
    const PlannedIncidents &planned_incidents
  )
      : Entity(
          position,
          14
        )
      , room_index_(room_index)
      , label_(label)
      , planned_incidents_(planned_incidents) {
  }

  const GeneratorState &GeneratorEntity::currentState() const {
    switch(state_) {
      case GeneratorStateEnum::offline:
      case GeneratorStateEnum::charging: return charging_state_;
      case GeneratorStateEnum::jammed:   return jammed_state_;
      case GeneratorStateEnum::broken:   return broken_state_;
      case GeneratorStateEnum::online:   return online_state_;
    }

    return charging_state_;
  }

  GeneratorState &GeneratorEntity::currentState() {
    switch(state_) {
      case GeneratorStateEnum::offline:
      case GeneratorStateEnum::charging: return charging_state_;
      case GeneratorStateEnum::jammed:   return jammed_state_;
      case GeneratorStateEnum::broken:   return broken_state_;
      case GeneratorStateEnum::online:   return online_state_;
    }

    return charging_state_;
  }

  void GeneratorEntity::update(
    GameContextDto &context,
    float           seconds_to_charge
  ) {
    const float delta = context.clock.deltaSeconds();

    advanceAnimation(delta);
    machinery_.rotor_angle += machinery_.rotor_speed * delta;
    skill_check_.tickResult(delta);

    const bool is_player_near =
      distanceTo(context.player.position()) < interaction_distance;

    refreshRepairStatus(context, is_player_near);

    currentState().update(
      *this,
      context,
      GeneratorUpdateDto {
        .is_player_near    = is_player_near,
        .seconds_to_charge = seconds_to_charge,
      }
    );

    if(!isOnline() && (is_being_repaired_ || progress_ > 0))
      emitChargeLight(context);
  }

  void GeneratorEntity::refreshRepairStatus(
    const GameContextDto &context,
    bool                  is_player_near
  ) {
    const PlayerEntity &player = context.player;

    was_repaired_last_frame_ = is_being_repaired_;

    is_being_repaired_ = currentState().canBeRepaired() &&
                         is_player_near &&
                         player.isInteracting() &&
                         player.canBeTargeted();
  }

  void GeneratorEntity::emitChargeLight(
    GameContextDto &context
  ) const {
    LightProps light;

    light.position      = position();
    light.radius        = 46;
    light.intensity     = 0.25F + (progress_ * 0.7F);
    light.color         = ColorProps::hex(0xffb050);
    light.flicker       = 0.35F;
    light.casts_shadows = false;
    context.lighting.add(light);
  }

  void GeneratorEntity::playSound(
    GameContextDto &context,
    SoundEnum       sound,
    float           volume,
    float           pitch
  ) const {
    context.sound_player
      .playAt(sound, volume, context.player.position(), position(), pitch);
  }

  void GeneratorEntity::triggerCrossedIncidents(
    GameContextDto &context,
    float           previous_progress
  ) {
    for(
      std::size_t incident_index = 0;
      incident_index < incident_thresholds.size();
      ++incident_index
    ) {
      const float threshold = incident_thresholds[incident_index];

      if(
        triggered_incidents_[incident_index] ||
        previous_progress >=
        threshold ||
        progress_ < threshold
      )
        continue;

      triggered_incidents_[incident_index] = true;
      triggerIncident(context, planned_incidents_[incident_index]);
    }
  }

  void GeneratorEntity::triggerIncident(
    GameContextDto       &context,
    GeneratorIncidentEnum incident
  ) {
    skill_check_.cancel();

    switch(incident) {
      case GeneratorIncidentEnum::none: break;
      case GeneratorIncidentEnum::jam:
        state_ = GeneratorStateEnum::jammed;
        jammed_state_.enter(*this, context);
        break;
      case GeneratorIncidentEnum::breakdown:
        state_ = GeneratorStateEnum::broken;
        broken_state_.enter(*this, context);
        break;
    }
  }

  void GeneratorEntity::bringOnline(
    GameContextDto &context
  ) {
    state_ = GeneratorStateEnum::online;
    online_state_.enter();
    skill_check_.cancel();
    context.world.powerRoom(room_index_, true);
    context.world.shake(0.35F);

    context.effects
      .emitSparks(context.world, position(), 30, Palette::neon_cyan);

    playSound(context, SoundEnum::generator_online, 1.0F, 1);

    context.sound_player.playAt(
      SoundEnum::led_power_up,
      0.8F,
      context.player.position(),
      context.world.conduitPoint(room_index_),
      1
    );

    context.hud.announce(
      Palette::success,
      std::format("GENERATOR {} ONLINE  -  THE ROOM IS LIGHTING UP", label_)
    );

    context.mobs.onGeneratorOnline(context);
  }

  void GeneratorEntity::forceOnline(
    GameContextDto &context
  ) {
    progress_ = 1;
    state_    = GeneratorStateEnum::online;
    context.world.powerRoom(room_index_, true);
  }

  void GeneratorEntity::draw(
    GameContextDto &context
  ) const {
    Renderer               &renderer     = context.renderer;
    const SpriteSheetProps &sprites      = context.art.sprites();
    const float             real_seconds = context.clock.realSeconds();
    const Vec2Props         center       = position();

    renderer.drawShadowDisc(
      center.add(Vec2Props::of(2, 4)),
      22,
      ColorProps::black().withAlpha(0.6F),
      1.2F
    );

    renderer.drawSprite(sprites.generator_base, center);

    renderer.drawSprite(
      sprites.generator_rotor,
      center,
      SpriteDrawDto {.rotation = machinery_.rotor_angle}
    );

    const ColorProps status_color = statusColor();
    const float core_level = isOnline() ? 0.8F : 0.12F + (progress_ * 0.35F);

    const float pulse =
      0.85F + (0.15F * std::sin(real_seconds * (3 + machinery_.rotor_speed)));

    renderer.drawGlowSprite(
      sprites.generator_glow,
      center,
      status_color.withAlpha(core_level * pulse)
    );

    renderer.drawGlowSprite(
      sprites.generator_rotor_glow,
      center,
      status_color.withAlpha(core_level * 0.9F),
      SpriteDrawDto {.rotation = machinery_.rotor_angle}
    );

    if(is_being_repaired_)
      renderer.drawGlowDisc(
        center,
        26,
        Palette::flashlight_white.withAlpha(0.12F),
        1.5F
      );
    if(!isOnline()) drawProgressRing(context, status_color);
    currentState().drawOverlay(*this, context);
  }

  void GeneratorEntity::drawProgressRing(
    GameContextDto &context,
    ColorProps      color
  ) const {
    const float lit_dots = progress_ * static_cast<float>(progress_ring_dots);

    for(int dot = 0; dot < progress_ring_dots; ++dot) {
      const float fraction =
        static_cast<float>(dot) / static_cast<float>(progress_ring_dots);

      const float angle =
        (fraction * ScalarMath::tau) - (ScalarMath::pi / 2.0F);

      const Vec2Props point =
        position().add(Vec2Props::fromAngle(angle).scale(progress_ring_radius));

      const bool is_lit = static_cast<float>(dot) < lit_dots;

      const bool is_threshold =
        std::ranges::any_of(incident_thresholds, [fraction](float threshold) {
          return std::abs(fraction - threshold) <
                 0.5F /
                 static_cast<float>(progress_ring_dots);
        });

      ColorProps dot_color = ColorProps::hex(0x202838);
      if(is_threshold) dot_color = Palette::spark_yellow.withAlpha(0.6F);
      if(is_lit) dot_color = color;

      context.renderer.drawGlowRectangle(
        point,
        Vec2Props::of(2.2F, 2.2F),
        dot_color.withAlpha(is_lit ? 1.0F : 0.85F)
      );
    }
  }
} // namespace wheel_of_fear
