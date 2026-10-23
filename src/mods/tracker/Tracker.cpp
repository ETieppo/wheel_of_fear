#include "mods/tracker/Tracker.hpp"

#include <format>
#include <string>

#include "core/audio/SoundPlayer.hpp"
#include "core/input/Input.hpp"
#include "core/items/ItemCatalog.hpp"
#include "core/math/Scalar.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/Art.hpp"
#include "mods/generators/Generators.hpp"
#include "mods/mobs/Mobs.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/player/Player.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  Tracker::Tracker()
      : last_ping_seconds_(
          signal_capacity,
          -10.0F
        ) {
    signals_.reserve(signal_capacity);
  }

  void Tracker::close() {
    is_open_     = false;
    open_amount_ = 0;
  }

  void Tracker::update(GameContextDto &context) {
    if(!context.clock.isRunning()) return;
    const float delta = context.clock.deltaSeconds();
    if(context.input.wasPressed(InputActionEnum::toggle_tracker) &&
       !context.player.isDead()) {
      is_open_ = !is_open_;
      context.sound_player.play(
        is_open_ ? SoundEnum::tracker_open : SoundEnum::tracker_close,
        0.7F,
        0,
        1
      );
    }
    open_amount_ = Scalar::approach(
      open_amount_, is_open_ ? 1.0F : 0.0F, delta * opening_speed
    );
    collectSignals(context);
    updateGhosts(context);
    if(is_open_) sweep(context);
  }

  void Tracker::sweep(GameContextDto &context) {
    const float previous_angle = sweep_angle_;
    sweep_angle_               = Scalar::floorModulo(
      sweep_angle_ + context.clock.deltaSeconds() * sweep_speed, Scalar::tau
    );
    const Vec2 player_position = context.player.position();
    for(std::size_t index = 0; index < signals_.size(); ++index) {
      const SignalProps &signal = signals_[index];
      const Vec2         offset = signal.position.sub(player_position);
      if(offset.length() > scan_range) continue;
      const float bearing =
        Scalar::floorModulo(offset.angle() + Scalar::tau, Scalar::tau);
      if(!crossedAngle(previous_angle, sweep_angle_, bearing)) continue;
      last_ping_seconds_[index] = context.clock.realSeconds();
      const float pitch =
        signal.kind == SignalKindEnum::unidentified ? 0.8F : 1.0F;
      context.sound_player.play(
        SoundEnum::tracker_blip,
        0.35F,
        Scalar::clamp(offset.x / 300, -1, 1),
        pitch
      );
      context.noise.emit(
        player_position, beep_noise_radius, NoiseSourceEnum::tracker_beep
      );
    }
  }

  bool Tracker::crossedAngle(
    float previous,
    float current,
    float target
  ) {
    if(current >= previous) return target > previous && target <= current;
    return target > previous || target <= current;
  }

  void Tracker::addSignal(const SignalProps &signal) {
    if(signals_.size() >= signal_capacity) return;
    signals_.push_back(signal);
  }

  void Tracker::collectSignals(const GameContextDto &context) {
    signals_.clear();
    const Generators &generators     = context.generators;
    const auto       &all_generators = generators.all();
    for(std::size_t index = 0; index < all_generators.size(); ++index) {
      const Generator &generator = all_generators[index];
      Color            color     = Palette::tracker_green;
      if(generator.state() ==
         GeneratorStateEnum::jammed ||
         generator.state() == GeneratorStateEnum::broken)
        color = Palette::spark_yellow;
      if(generator.isOnline()) color = Palette::success;
      addSignal(
        SignalProps {
          SignalKindEnum::generator,
          generator.position(),
          generator.label(),
          color,
          std::nullopt,
          static_cast<std::uint32_t>(index)
        }
      );
    }
    const auto &fabricators = generators.fabricators();
    for(std::size_t index = 0; index < fabricators.size(); ++index) {
      const Fabricator &fabricator = fabricators[index];
      const ItemEnum    item       = fabricator.producedItem();
      addSignal(
        SignalProps {
          SignalKindEnum::fabricator,
          fabricator.position(),
          '?',
          ItemCatalog::of(item).color(),
          item,
          static_cast<std::uint32_t>(100 + index)
        }
      );
    }
    const Color exit_color =
      generators.exitDoor().isUnlocked() ? Palette::success : Palette::danger;
    addSignal(
      SignalProps {
        SignalKindEnum::exit,
        context.world.exitPosition(),
        '?',
        exit_color,
        std::nullopt,
        200
      }
    );
    const auto &mobs = context.mobs.all();
    for(std::size_t index = 0; index < mobs.size(); ++index) {
      if(!mobs[index]->isDisguised()) continue;
      const auto disguise = static_cast<ItemEnum>(index % item_count);
      addSignal(
        SignalProps {
          SignalKindEnum::fabricator,
          mobs[index]->position(),
          '?',
          ItemCatalog::of(disguise).color(),
          disguise,
          static_cast<std::uint32_t>(300 + index)
        }
      );
    }
    for(std::size_t index = 0; index < ghosts_.size(); ++index) {
      addSignal(
        SignalProps {
          SignalKindEnum::unidentified,
          ghosts_[index].position,
          '?',
          Palette::tracker_green.withAlpha(0.7F),
          std::nullopt,
          static_cast<std::uint32_t>(400 + index)
        }
      );
    }
  }

  void Tracker::updateGhosts(GameContextDto &context) {
    const float delta = context.clock.deltaSeconds();
    for(GhostEchoProps &ghost : ghosts_) {
      ghost.remaining_seconds -= delta;
      ghost.position = ghost.position.add(ghost.drift.scale(delta));
    }
    std::erase_if(ghosts_, [](const GhostEchoProps &ghost) {
      return ghost.remaining_seconds <= 0;
    });
    RandomGenerator &random = context.world.random();
    const float      ghost_rate =
      0.05F + 0.02F * static_cast<float>(context.world.phase());
    if(ghosts_.size() >= ghost_capacity || !random.chance(delta * ghost_rate))
      return;
    GhostEchoProps ghost;
    const Vec2     bearing = random.direction();
    ghost.position         = context.player.position().add(
      bearing.scale(random.range(120, scan_range))
    );
    const Vec2 drift_direction = random.direction();
    ghost.drift                = drift_direction.scale(random.range(4, 18));
    ghost.remaining_seconds    = random.range(6, 14);
    ghosts_.push_back(ghost);
  }

  Vec2 Tracker::worldToRadar(Vec2 offset) {
    return radar_center.add(offset.scale(radar_radius / scan_range));
  }

  void Tracker::draw(GameContextDto &context) const {
    if(open_amount_ <= 0.01F) return;
    Renderer   &renderer = context.renderer;
    const Color green    = Palette::tracker_green;
    renderer.drawUiShape(
      QuadKindEnum::crt_screen,
      Vec2::of(RenderConfig::ui_width / 2, RenderConfig::ui_height / 2),
      Vec2::of(RenderConfig::ui_width * 6, RenderConfig::ui_height * 6),
      green.withAlpha(open_amount_),
      0,
      0
    );
    if(open_amount_ < 0.6F) return;
    drawWallEchoes(context);
    for(float ring = 1; ring <= 4; ring += 1) {
      const float ring_size = radar_radius * 2 * ring / 4;
      renderer.drawUiShape(
        QuadKindEnum::ring,
        radar_center,
        Vec2::of(ring_size, ring_size),
        green.withAlpha(0.18F),
        0,
        0.012F * 4 / ring
      );
    }
    renderer.drawUiRectangle(
      radar_center.x - radar_radius,
      radar_center.y,
      radar_radius * 2,
      1,
      green.withAlpha(0.12F)
    );
    renderer.drawUiRectangle(
      radar_center.x,
      radar_center.y - radar_radius,
      1,
      radar_radius * 2,
      green.withAlpha(0.12F)
    );
    for(float trail = 0; trail < 14; trail += 1) {
      const float angle = sweep_angle_ - trail * 0.035F;
      const Vec2  tip =
        radar_center.add(Vec2::fromAngle(angle).scale(radar_radius / 2));
      renderer.drawUiShape(
        QuadKindEnum::solid,
        tip,
        Vec2::of(radar_radius, 2),
        green.withAlpha(0.5F * (1 - trail / 14)),
        angle,
        0
      );
    }
    for(std::size_t index = 0; index < signals_.size(); ++index)
      drawSignal(context, signals_[index], last_ping_seconds_[index]);
    renderer.drawUiShape(
      QuadKindEnum::solid,
      radar_center,
      Vec2::of(8, 3),
      Color::white(),
      context.player.aimDirection().angle(),
      0
    );
    renderer.drawUiShape(
      QuadKindEnum::soft_disc,
      radar_center,
      Vec2::of(10, 10),
      Color::white().withAlpha(0.6F),
      0,
      1
    );
    renderer.drawText(
      std::format("TK-7 TECH TRACKER  \xC2\xB7  {} SIGNALS", signals_.size()),
      30,
      22,
      18,
      green,
      TextAlignEnum::left
    );
    renderer.drawText(
      "[TAB] CLOSE  -  YOUR LIGHT IS DOWN WHILE THE SCREEN IS UP",
      30,
      504,
      14,
      green.withAlpha(0.7F),
      TextAlignEnum::left
    );
    renderer.drawText(
      "?  UNIDENTIFIED SIGNAL",
      RenderConfig::ui_width - 30,
      504,
      14,
      green.withAlpha(0.7F),
      TextAlignEnum::right
    );
  }

  void Tracker::drawWallEchoes(GameContextDto &context) const {
    const TileMap &map             = context.world.map();
    const Vec2     player_position = context.player.position();
    const auto reach = static_cast<int>(scan_range / RenderConfig::tile_size);
    const TileCoordinateProps center = TileMap::tileAt(player_position);
    for(int y = center.y - reach; y <= center.y + reach; ++y) {
      for(int x = center.x - reach; x <= center.x + reach; ++x) {
        if(!map.contains(x, y) || !map.isWall(x, y)) continue;
        const bool borders_floor = !map.isWall(x + 1, y) ||
                                   !map.isWall(x - 1, y) ||
                                   !map.isWall(x, y + 1) ||
                                   !map.isWall(x, y - 1);
        if(!borders_floor) continue;
        const Vec2 offset = TileMap::tileCenter(x, y).sub(player_position);
        if(offset.length() > scan_range) continue;
        const Vec2 point = worldToRadar(offset);
        context.renderer.drawUiRectangle(
          point.x - 1.5F,
          point.y - 1.5F,
          3,
          3,
          Palette::tracker_green.withAlpha(0.13F)
        );
      }
    }
  }

  void Tracker::drawSignal(
    GameContextDto    &context,
    const SignalProps &signal,
    float              last_ping
  ) const {
    Renderer   &renderer     = context.renderer;
    const float real_seconds = context.clock.realSeconds();
    const Vec2  offset       = signal.position.sub(context.player.position());
    const float distance     = offset.length();
    const float jitter_strength = distance * 0.04F;
    const auto  identity        = static_cast<float>(signal.identity);
    const Vec2  jitter =
      Vec2::of(
        Scalar::hashTime(real_seconds + identity, 3) - 0.5F,
        Scalar::hashTime(real_seconds + identity + 0.5F, 3) - 0.5F
      )
        .scale(jitter_strength);
    const float glow = Scalar::clamp(
      1 - (real_seconds - last_ping) / blip_fade_seconds, 0.25F, 1
    );
    if(distance > scan_range) {
      const Vec2 edge =
        radar_center.add(offset.normalized().scale(radar_radius + 14));
      renderer.drawUiShape(
        QuadKindEnum::solid,
        edge,
        Vec2::of(10, 4),
        signal.color.withAlpha(0.8F),
        offset.angle(),
        0
      );
      return;
    }
    const Vec2 point = worldToRadar(offset.add(jitter));
    renderer.drawUiShape(
      QuadKindEnum::soft_disc,
      point,
      Vec2::of(34, 34),
      signal.color.withAlpha(0.35F * glow),
      0,
      2
    );
    switch(signal.kind) {
      case SignalKindEnum::generator:
        renderer.drawUiShape(
          QuadKindEnum::solid,
          point,
          Vec2::of(12, 12),
          signal.color.withAlpha(glow),
          0,
          0
        );
        renderer.drawUiShape(
          QuadKindEnum::solid,
          point,
          Vec2::of(8, 8),
          Color::black().withAlpha(0.7F),
          0,
          0
        );
        renderer.drawText(
          std::string(1, signal.label),
          point.x + 10,
          point.y - 14,
          16,
          signal.color.withAlpha(glow),
          TextAlignEnum::left
        );
        break;
      case SignalKindEnum::fabricator:
        renderer.drawUiShape(
          QuadKindEnum::solid,
          point,
          Vec2::of(11, 11),
          signal.color.withAlpha(glow),
          Scalar::pi / 4.0F,
          0
        );
        if(signal.item)
          renderer.drawUiSprite(
            context.art.sprites().itemIcon(*signal.item),
            point.add(Vec2::of(16, -10)),
            14,
            Color::white().withAlpha(glow)
          );
        break;
      case SignalKindEnum::exit:
        renderer.drawUiShape(
          QuadKindEnum::ring,
          point,
          Vec2::of(18, 18),
          signal.color.withAlpha(glow),
          0,
          0.25F
        );
        renderer.drawText(
          "EXIT",
          point.x + 12,
          point.y - 14,
          15,
          signal.color.withAlpha(glow),
          TextAlignEnum::left
        );
        break;
      case SignalKindEnum::unidentified:
        renderer.drawUiShape(
          QuadKindEnum::ring,
          point,
          Vec2::of(14, 14),
          signal.color.withAlpha(glow * 0.8F),
          0,
          0.3F
        );
        renderer.drawText(
          "?",
          point.x + 9,
          point.y - 13,
          15,
          signal.color.withAlpha(glow),
          TextAlignEnum::left
        );
        break;
    }
  }
} // namespace wheel_of_fear
