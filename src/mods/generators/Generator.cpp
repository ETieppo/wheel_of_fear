#include "mods/generators/Generator.hpp"

#include <algorithm>
#include <cmath>
#include <format>

#include "core/audio/SoundPlayer.hpp"
#include "core/input/Input.hpp"
#include "core/items/ItemCatalog.hpp"
#include "core/math/Scalar.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/Art.hpp"
#include "mods/effects/Effects.hpp"
#include "mods/hud/Hud.hpp"
#include "mods/lighting/Lighting.hpp"
#include "mods/mobs/Mobs.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/player/Player.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  namespace {
    constexpr float interaction_distance      = 36;
    constexpr float wiring_repair_seconds     = 2.2F;
    constexpr float regression_delay_seconds  = 2.5F;
    constexpr float regression_per_second     = 0.025F;
    constexpr float overload_restart_progress = 0.75F;
    constexpr float repair_noise_radius       = 230;
    constexpr float failure_noise_radius      = 430;
    constexpr float failure_regression        = 0.14F;
    constexpr float perfect_bonus             = 0.05F;
    constexpr float failure_breakdown_chance  = 0.3F;
    constexpr int   progress_ring_dots        = 24;
    constexpr float progress_ring_radius      = 23;
  } // namespace

  Generator::Generator(
    Vec2                    position,
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

  void Generator::update(
    GameContextDto &context,
    float           seconds_to_charge
  ) {
    const float delta = context.clock.deltaSeconds();
    advanceAnimation(delta);
    rotor_angle_ += rotor_speed_ * delta;
    skill_check_.tickResult(delta);
    const Player &player = context.player;
    const bool    is_player_near =
      distanceTo(player.position()) < interaction_distance;
    was_repaired_last_frame_   = is_being_repaired_;
    const bool can_be_repaired = state_ ==
                                 GeneratorStateEnum::offline ||
                                 state_ ==
                                 GeneratorStateEnum::charging ||
                                 state_ == GeneratorStateEnum::broken;
    is_being_repaired_         = can_be_repaired &&
                                 is_player_near &&
                                 player.isInteracting() &&
                                 player.canBeTargeted();
    switch(state_) {
      case GeneratorStateEnum::offline:
      case GeneratorStateEnum::charging:
        updateCharging(context, is_player_near, seconds_to_charge);
        break;
      case GeneratorStateEnum::jammed:
        updateJammed(context, is_player_near);
        break;
      case GeneratorStateEnum::broken:
        updateBroken(context, is_player_near);
        break;
      case GeneratorStateEnum::online: updateOnline(context); break;
    }
    if(
      state_ !=
      GeneratorStateEnum::online &&
      (is_being_repaired_ || progress_ > 0)
    ) {
      LightProps light;
      light.position      = position();
      light.radius        = 46;
      light.intensity     = 0.25F + progress_ * 0.7F;
      light.color         = Color::hex(0xffb050);
      light.flicker       = 0.35F;
      light.casts_shadows = false;
      context.lighting.add(light);
    }
  }

  void Generator::overload(
    GameContextDto &context
  ) {
    state_                 = GeneratorStateEnum::charging;
    progress_              = overload_restart_progress;
    seconds_online_        = 0;
    seconds_unattended_    = 0;
    has_warned_regression_ = false;
    context.world.powerRoom(room_index_, false);
    context.effects
      .emitSparks(context.world, position(), 22, Palette::spark_blue);
    playSound(context, SoundEnum::generator_breakdown, 0.9F, 0.8F);
    context.noise
      .emit(position(), failure_noise_radius, NoiseSourceEnum::mob_incident);
    context.hud.announce(
      Palette::danger,
      std::format("GENERATOR {} OVERLOADED  -  POWER IS DRAINING", label_)
    );
  }

  void Generator::playSound(
    GameContextDto &context,
    SoundEnum       sound,
    float           volume,
    float           pitch
  ) const {
    context.sound_player
      .playAt(sound, volume, context.player.position(), position(), pitch);
  }

  void Generator::updateCharging(
    GameContextDto &context,
    bool            is_player_near,
    float           seconds_to_charge
  ) {
    const float delta = context.clock.deltaSeconds();
    if(!is_being_repaired_) {
      if(was_repaired_last_frame_ && skill_check_.isActive())
        resolveSkillCheck(context, SkillCheckResultEnum::failed);
      skill_check_.cancel();
      rotor_speed_ = Scalar::approach(rotor_speed_, 0, delta * 3);
      if(state_ == GeneratorStateEnum::charging && progress_ > 0)
        regressWhileUnattended(context);
      if(is_player_near && context.player.canBeTargeted())
        context.hud.showPrompt(
          InputActionEnum::interact,
          std::format("hold to charge generator {}", label_)
        );
      return;
    }
    context.player.beginRepair(position());
    state_                        = GeneratorStateEnum::charging;
    seconds_unattended_           = 0;
    has_warned_regression_        = false;
    const float previous_progress = progress_;
    progress_    = std::min(1.0F, progress_ + delta / seconds_to_charge);
    rotor_speed_ = Scalar::approach(rotor_speed_, 2 + progress_ * 7, delta * 6);
    ratchet_timer_ -= delta * (1 + progress_ * 2.5F);
    if(ratchet_timer_ <= 0) {
      ratchet_timer_ = 0.34F;
      playSound(
        context,
        SoundEnum::generator_tick,
        0.5F,
        0.85F + progress_ * 0.5F
      );
      context.noise
        .emit(position(), repair_noise_radius, NoiseSourceEnum::repairing);
    }
    updateSkillCheck(context);
    if(state_ != GeneratorStateEnum::charging) return;
    context.hud.showPrompt(
      std::nullopt,
      std::format("CHARGING GENERATOR {}  {:.0f}%", label_, progress_ * 100)
    );
    triggerCrossedIncidents(context, previous_progress);
    if(state_ == GeneratorStateEnum::charging && progress_ >= 1)
      bringOnline(context);
  }

  void Generator::regressWhileUnattended(
    GameContextDto &context
  ) {
    const float delta = context.clock.deltaSeconds();
    seconds_unattended_ += delta;
    if(seconds_unattended_ <= regression_delay_seconds) return;
    progress_ = std::max(0.0F, progress_ - delta * regression_per_second);
    if(!has_warned_regression_) {
      has_warned_regression_ = true;
      context.hud.announce(
        Palette::spark_yellow,
        std::format("GENERATOR {} IS LOSING CHARGE", label_)
      );
    }
    RandomGenerator &random = context.world.random();
    if(random.chance(delta * 1.5F))
      context.effects.emitSparks(
        context.world,
        position().add(Vec2::of(random.range(-10, 10), -6)),
        2,
        Palette::spark_yellow
      );
  }

  void Generator::triggerCrossedIncidents(
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

  void Generator::updateSkillCheck(
    GameContextDto &context
  ) {
    const float delta = context.clock.deltaSeconds();
    if(!skill_check_.isActive()) {
      if(!skill_check_.tickCountdown(delta)) return;
      skill_check_.begin(context.world.phase(), context.world.random());
      context.sound_player.play(SoundEnum::qte_alert, 0.7F, 0, 1);
      return;
    }
    const auto result = skill_check_.advanceNeedle(
      delta,
      context.input.wasPressed(InputActionEnum::hold_breath)
    );
    if(result) resolveSkillCheck(context, *result);
  }

  void Generator::resolveSkillCheck(
    GameContextDto      &context,
    SkillCheckResultEnum result
  ) {
    skill_check_.resolve(result, context.world.random());
    switch(result) {
      case SkillCheckResultEnum::perfect:
        progress_ = std::min(0.999F, progress_ + perfect_bonus);
        context.sound_player.play(SoundEnum::qte_success, 0.7F, 0, 1.25F);
        break;
      case SkillCheckResultEnum::good:
        context.sound_player.play(SoundEnum::qte_success, 0.55F, 0, 1);
        break;
      case SkillCheckResultEnum::failed:
        progress_ = std::max(0.0F, progress_ - failure_regression);
        context.sound_player.play(SoundEnum::qte_fail, 0.9F, 0, 1);
        playSound(context, SoundEnum::electric_buzz, 0.7F, 1.2F);
        context.effects
          .emitSparks(context.world, position(), 16, Palette::spark_blue);
        context.noise.emit(
          position(),
          failure_noise_radius,
          NoiseSourceEnum::repair_failure
        );
        context.world.shake(0.4F);
        if(
          progress_ >
          0.1F &&
          context.world.random().chance(failure_breakdown_chance)
        ) {
          triggerIncident(context, GeneratorIncidentEnum::breakdown);
        } else {
          context.hud.announce(
            Palette::danger,
            std::format("SHORT CIRCUIT  -  GENERATOR {} LOST CHARGE", label_)
          );
        }
        break;
      case SkillCheckResultEnum::none: break;
    }
  }

  void Generator::updateJammed(
    GameContextDto &context,
    bool            is_player_near
  ) {
    RandomGenerator &random = context.world.random();
    rotor_speed_ =
      Scalar::approach(rotor_speed_, 0, context.clock.deltaSeconds() * 6);
    spark_timer_ -= context.clock.deltaSeconds();
    if(spark_timer_ <= 0) {
      spark_timer_ = random.range(0.6F, 1.6F);
      context.effects.emitSparks(
        context.world,
        position().add(Vec2::of(-4, -2)),
        3,
        Palette::spark_yellow
      );
    }
    if(!is_player_near) return;
    const std::string_view item_name =
      ItemCatalog::of(required_item_).displayName();
    Inventory &inventory = context.player.inventory();
    if(!inventory.has(required_item_)) {
      context.hud.showPrompt(
        std::nullopt,
        std::format(
          "GENERATOR {} NEEDS A {}  -  FIND A FABRICATOR",
          label_,
          item_name
        )
      );
      return;
    }
    context.hud.showPrompt(
      InputActionEnum::interact,
      std::format("insert {} into generator {}", item_name, label_)
    );
    if(!context.input.wasPressed(InputActionEnum::interact)) return;
    inventory.consume(required_item_);
    state_ = GeneratorStateEnum::charging;
    playSound(context, SoundEnum::part_pickup, 0.7F, 0.8F);
    playSound(context, SoundEnum::generator_tick, 0.8F, 0.6F);
    context.hud.announce(
      Palette::success,
      std::format("GENERATOR {} UNJAMMED", label_)
    );
  }

  void Generator::updateBroken(
    GameContextDto &context,
    bool            is_player_near
  ) {
    const float      delta  = context.clock.deltaSeconds();
    RandomGenerator &random = context.world.random();
    rotor_speed_            = Scalar::approach(rotor_speed_, 0, delta * 8);
    spark_timer_ -= delta;
    if(spark_timer_ <= 0) {
      spark_timer_         = random.range(0.08F, 0.5F);
      const float offset_x = random.range(-10, 10);
      const float offset_y = random.range(-10, 6);
      context.effects.emitSparks(
        context.world,
        position().add(Vec2::of(offset_x, offset_y)),
        4,
        Palette::spark_blue
      );
      if(random.chance(0.3F))
        playSound(
          context,
          SoundEnum::electric_buzz,
          0.4F,
          random.range(0.8F, 1.3F)
        );
    }
    LightProps light;
    light.position  = position();
    light.radius    = 50;
    light.intensity = 0.6F;
    light.color     = Palette::spark_blue;
    light.flicker   = 0.9F;
    context.lighting.add(light);
    if(!is_player_near) return;
    if(!is_being_repaired_) {
      context.hud.showPrompt(
        InputActionEnum::interact,
        std::format("hold to rewire generator {}", label_)
      );
      return;
    }
    context.player.beginRepair(position());
    wiring_progress_ += delta;
    repair_sound_timer_ -= delta;
    if(repair_sound_timer_ <= 0) {
      repair_sound_timer_ = 0.5F;
      playSound(
        context,
        SoundEnum::generator_tick,
        0.7F,
        0.6F + wiring_progress_
      );
      context.noise
        .emit(position(), repair_noise_radius, NoiseSourceEnum::repairing);
    }
    context.hud.showPrompt(
      std::nullopt,
      std::format(
        "REWIRING GENERATOR {}  {:.0f}%",
        label_,
        wiring_progress_ / wiring_repair_seconds * 100
      )
    );
    if(wiring_progress_ < wiring_repair_seconds) return;
    wiring_progress_ = 0;
    state_           = GeneratorStateEnum::charging;
    context.hud.announce(
      Palette::success,
      std::format("GENERATOR {} REWIRED", label_)
    );
    playSound(context, SoundEnum::part_pickup, 0.5F, 0.6F);
  }

  void Generator::updateOnline(
    GameContextDto &context
  ) {
    const float delta = context.clock.deltaSeconds();
    seconds_online_ += delta;
    rotor_speed_        = Scalar::approach(rotor_speed_, 9, delta * 2);
    const float warm_up = Scalar::clamp(seconds_online_ / 1.5F, 0, 1);
    LightProps  light;
    light.position      = position();
    light.radius        = 70 * warm_up;
    light.intensity     = 0.9F;
    light.color         = Color::hex(0x7af0ff);
    light.flicker       = 0.04F;
    light.casts_shadows = false;
    context.lighting.add(light);
  }

  void Generator::triggerIncident(
    GameContextDto       &context,
    GeneratorIncidentEnum incident
  ) {
    skill_check_.cancel();
    switch(incident) {
      case GeneratorIncidentEnum::none: break;
      case GeneratorIncidentEnum::jam:
        {
          state_           = GeneratorStateEnum::jammed;
          const auto parts = ItemCatalog::generatorParts();
          required_item_   = parts[context.world.random().index(parts.size())];
          context.effects
            .emitSparks(context.world, position(), 10, Palette::spark_yellow);
          playSound(context, SoundEnum::generator_request, 0.9F, 1);
          context.hud.announce(
            Palette::spark_yellow,
            std::format(
              "GENERATOR {} JAMMED  -  IT NEEDS A {}",
              label_,
              ItemCatalog::of(required_item_).displayName()
            )
          );
          break;
        }
      case GeneratorIncidentEnum::breakdown:
        state_           = GeneratorStateEnum::broken;
        progress_        = std::max(0.0F, progress_ - 0.12F);
        wiring_progress_ = 0;
        context.world.shake(0.4F);
        context.effects
          .emitSparks(context.world, position(), 24, Palette::spark_blue);
        playSound(context, SoundEnum::generator_breakdown, 1.0F, 1);
        context.noise.emit(
          position(),
          failure_noise_radius,
          NoiseSourceEnum::repair_failure
        );
        context.hud.announce(
          Palette::danger,
          std::format("GENERATOR {} BROKE DOWN  -  HOLD E TO REWIRE", label_)
        );
        break;
    }
  }

  void Generator::bringOnline(
    GameContextDto &context
  ) {
    state_          = GeneratorStateEnum::online;
    seconds_online_ = 0;
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

  void Generator::forceOnline(
    GameContextDto &context
  ) {
    progress_ = 1;
    state_    = GeneratorStateEnum::online;
    context.world.powerRoom(room_index_, true);
  }

  Color Generator::statusColor() const {
    switch(state_) {
      case GeneratorStateEnum::online:   return Palette::neon_cyan;
      case GeneratorStateEnum::charging: return Palette::neon_amber;
      case GeneratorStateEnum::jammed:   return Palette::spark_yellow;
      case GeneratorStateEnum::broken:   return Palette::spark_blue;
      case GeneratorStateEnum::offline:  return Palette::neon_red;
    }
    return Palette::neon_red;
  }

  void Generator::draw(
    GameContextDto &context
  ) const {
    Renderer               &renderer     = context.renderer;
    const SpriteSheetProps &sprites      = context.art.sprites();
    const float             real_seconds = context.clock.realSeconds();
    const Vec2              center       = position();
    renderer.drawShadowDisc(
      center.add(Vec2::of(2, 4)),
      22,
      Color::black().withAlpha(0.6F),
      1.2F
    );
    renderer.drawSprite(sprites.generator_base, center);
    renderer.drawSprite(
      sprites.generator_rotor,
      center,
      SpriteDrawDto {.rotation = rotor_angle_}
    );
    const Color status_color = statusColor();
    const float core_level   = isOnline() ? 0.8F : 0.12F + progress_ * 0.35F;
    const float pulse =
      0.85F + 0.15F * std::sin(real_seconds * (3 + rotor_speed_));
    renderer.drawGlowSprite(
      sprites.generator_glow,
      center,
      status_color.withAlpha(core_level * pulse)
    );
    renderer.drawGlowSprite(
      sprites.generator_rotor_glow,
      center,
      status_color.withAlpha(core_level * 0.9F),
      SpriteDrawDto {.rotation = rotor_angle_}
    );
    if(is_being_repaired_)
      renderer.drawGlowDisc(
        center,
        26,
        Palette::flashlight_white.withAlpha(0.12F),
        1.5F
      );
    if(!isOnline()) drawProgressRing(context, status_color);
    if(state_ != GeneratorStateEnum::jammed) return;
    const float bob = std::sin(real_seconds * 4) * 2;
    renderer.drawGlowSprite(
      sprites.itemIcon(required_item_),
      center.add(Vec2::of(0, -32 + bob)),
      ItemCatalog::of(required_item_).color(),
      SpriteDrawDto {.scale = 1.2F}
    );
  }

  void Generator::drawProgressRing(
    GameContextDto &context,
    Color           color
  ) const {
    const float lit_dots = progress_ * static_cast<float>(progress_ring_dots);
    for(int dot = 0; dot < progress_ring_dots; ++dot) {
      const float fraction =
        static_cast<float>(dot) / static_cast<float>(progress_ring_dots);
      const float angle = fraction * Scalar::tau - Scalar::pi / 2.0F;
      const Vec2  point =
        position().add(Vec2::fromAngle(angle).scale(progress_ring_radius));
      const bool is_lit       = static_cast<float>(dot) < lit_dots;
      const bool is_threshold = std::any_of(
        incident_thresholds.begin(),
        incident_thresholds.end(),
        [fraction](float threshold) {
          return std::abs(fraction - threshold) <
                 0.5F /
                 static_cast<float>(progress_ring_dots);
        }
      );
      Color dot_color = Color::hex(0x202838);
      if(is_threshold) dot_color = Palette::spark_yellow.withAlpha(0.6F);
      if(is_lit) dot_color = color;
      context.renderer.drawGlowRectangle(
        point,
        Vec2::of(2.2F, 2.2F),
        dot_color.withAlpha(is_lit ? 1.0F : 0.85F)
      );
    }
  }
} // namespace wheel_of_fear
