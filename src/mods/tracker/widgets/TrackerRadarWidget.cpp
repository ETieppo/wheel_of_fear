#include "mods/tracker/widgets/TrackerRadarWidget.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/Renderer.hpp"
#include "core/text/Localization.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/tracker/TrackerConfig.hpp"
#include "mods/world/WorldSystem.hpp"
#include <format>
#include <string>

namespace wheel_of_fear {
  namespace {
    constexpr LocalizedTextProps tracker_title {
      "TK-7 TECH TRACKER  \xC2\xB7  {} SIGNALS",
      "RASTREADOR TK-7  \xC2\xB7  {} SINAIS",
    };
    constexpr LocalizedTextProps tracker_hint {
      "[TAB] CLOSE  -  YOUR LIGHT IS DOWN WHILE THE SCREEN IS UP",
      "[TAB] FECHAR  -  SUA LUZ FICA APAGADA COM A TELA ABERTA",
    };
    constexpr LocalizedTextProps unidentified_label {
      "?  UNIDENTIFIED SIGNAL",
      "?  SINAL NÃO IDENTIFICADO",
    };
    constexpr LocalizedTextProps exit_label {
      "EXIT",
      "SAÍDA",
    };
  } // namespace

  void TrackerRadarWidget::draw(
    GameContextDto       &context,
    const TrackerViewDto &view
  ) {
    if(view.open_amount <= 0.01F) return;

    context.renderer.drawUiShape(
      QuadKindEnum::crt_screen,
      Vec2Props::of(RenderConfig::ui_width / 2, RenderConfig::ui_height / 2),
      Vec2Props::of(RenderConfig::ui_width * 6, RenderConfig::ui_height * 6),
      Palette::tracker_green.withAlpha(view.open_amount),
      0,
      0
    );

    if(view.open_amount < 0.6F) return;
    drawWallEchoes(context);
    drawScope(context, view.sweep_angle);

    for(std::size_t index = 0; index < view.signals.size(); ++index)
      drawSignal(context, view.signals[index], view.last_ping_seconds[index]);

    drawPlayerMarker(context);
    drawCaptions(context, view.signals.size());
  }

  Vec2Props TrackerRadarWidget::worldToRadar(Vec2Props offset) {
    return TrackerConfig::radar_center.add(
      offset.scale(TrackerConfig::radar_radius / TrackerConfig::scan_range)
    );
  }

  void TrackerRadarWidget::drawScope(
    GameContextDto &context,
    float           sweep_angle
  ) {
    Renderer        &renderer = context.renderer;
    const ColorProps green    = Palette::tracker_green;
    const Vec2Props  center   = TrackerConfig::radar_center;
    const float      radius   = TrackerConfig::radar_radius;

    for(int ring_index = 1; ring_index <= 4; ++ring_index) {
      const auto  ring      = static_cast<float>(ring_index);
      const float ring_size = radius * 2 * ring / 4;

      renderer.drawUiShape(
        QuadKindEnum::ring,
        center,
        Vec2Props::of(ring_size, ring_size),
        green.withAlpha(0.18F),
        0,
        0.012F * 4 / ring
      );
    }

    renderer.drawUiRectangle(
      center.x - radius,
      center.y,
      radius * 2,
      1,
      green.withAlpha(0.12F)
    );

    renderer.drawUiRectangle(
      center.x,
      center.y - radius,
      1,
      radius * 2,
      green.withAlpha(0.12F)
    );

    for(int trail_index = 0; trail_index < 14; ++trail_index) {
      const auto      trail = static_cast<float>(trail_index);
      const float     angle = sweep_angle - (trail * 0.035F);
      const Vec2Props tip
        = center.add(Vec2Props::fromAngle(angle).scale(radius / 2));

      renderer.drawUiShape(
        QuadKindEnum::solid,
        tip,
        Vec2Props::of(radius, 2),
        green.withAlpha(0.5F * (1 - (trail / 14))),
        angle,
        0
      );
    }
  }

  void TrackerRadarWidget::drawWallEchoes(GameContextDto &context) {
    const TileMapModel &map             = context.world.map();
    const Vec2Props     player_position = context.player.position();
    const auto          reach
      = static_cast<int>(TrackerConfig::scan_range / RenderConfig::tile_size);
    const TileCoordinateProps center = TileMapModel::tileAt(player_position);

    for(int y = center.y - reach; y <= center.y + reach; ++y) {
      for(int x = center.x - reach; x <= center.x + reach; ++x) {
        if(!map.contains(x, y) || !map.isWall(x, y)) continue;

        const bool borders_floor = !map.isWall(x + 1, y)
          || !map.isWall(x - 1, y)
          || !map.isWall(x, y + 1)
          || !map.isWall(x, y - 1);

        if(!borders_floor) continue;
        const Vec2Props offset
          = TileMapModel::tileCenter(x, y).sub(player_position);

        if(offset.length() > TrackerConfig::scan_range) continue;
        const Vec2Props point = worldToRadar(offset);

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

  void TrackerRadarWidget::drawPlayerMarker(GameContextDto &context) {
    Renderer &renderer = context.renderer;

    renderer.drawUiShape(
      QuadKindEnum::solid,
      TrackerConfig::radar_center,
      Vec2Props::of(8, 3),
      ColorProps::white(),
      context.player.aimDirection().angle(),
      0
    );

    renderer.drawUiShape(
      QuadKindEnum::soft_disc,
      TrackerConfig::radar_center,
      Vec2Props::of(10, 10),
      ColorProps::white().withAlpha(0.6F),
      0,
      1
    );
  }

  void TrackerRadarWidget::drawCaptions(
    GameContextDto &context,
    std::size_t     signal_count
  ) {
    Renderer        &renderer = context.renderer;
    const ColorProps green    = Palette::tracker_green;

    renderer.drawText(
      Localization::format(tracker_title, signal_count),
      30,
      22,
      18,
      green,
      TextAlignEnum::left
    );

    renderer.drawText(
      Localization::text(tracker_hint),
      30,
      504,
      14,
      green.withAlpha(0.7F),
      TextAlignEnum::left
    );

    renderer.drawText(
      Localization::text(unidentified_label),
      RenderConfig::ui_width - 30,
      504,
      14,
      green.withAlpha(0.7F),
      TextAlignEnum::right
    );
  }

  void TrackerRadarWidget::drawSignal(
    GameContextDto    &context,
    const SignalProps &signal,
    float              last_ping
  ) {
    const float     real_seconds = context.clock.realSeconds();
    const Vec2Props offset   = signal.position.sub(context.player.position());
    const float     distance = offset.length();
    const float     jitter_strength = distance * 0.04F;
    const auto      identity        = static_cast<float>(signal.identity);
    const Vec2Props jitter
      = Vec2Props::of(
          ScalarMath::hashTime(real_seconds + identity, 3) - 0.5F,
          ScalarMath::hashTime(real_seconds + identity + 0.5F, 3) - 0.5F
      )
          .scale(jitter_strength);
    const float glow = ScalarMath::clamp(
      1 - ((real_seconds - last_ping) / blip_fade_seconds),
      0.25F,
      1
    );

    if(distance > TrackerConfig::scan_range) {
      drawOutOfRangeSignal(context, signal, offset);
      return;
    }
    const Vec2Props point = worldToRadar(offset.add(jitter));

    context.renderer.drawUiShape(
      QuadKindEnum::soft_disc,
      point,
      Vec2Props::of(34, 34),
      signal.color.withAlpha(0.35F * glow),
      0,
      2
    );

    drawSignalMarker(context, signal, point, glow);
  }

  void TrackerRadarWidget::drawOutOfRangeSignal(
    GameContextDto    &context,
    const SignalProps &signal,
    Vec2Props          offset
  ) {
    const Vec2Props edge = TrackerConfig::radar_center.add(
      offset.normalized().scale(TrackerConfig::radar_radius + 14)
    );

    context.renderer.drawUiShape(
      QuadKindEnum::solid,
      edge,
      Vec2Props::of(10, 4),
      signal.color.withAlpha(0.8F),
      offset.angle(),
      0
    );
  }

  void TrackerRadarWidget::drawSignalMarker(
    GameContextDto    &context,
    const SignalProps &signal,
    Vec2Props          point,
    float              glow
  ) {
    Renderer &renderer = context.renderer;

    switch(signal.kind) {
      case SignalKindEnum::generator:
        renderer.drawUiShape(
          QuadKindEnum::solid,
          point,
          Vec2Props::of(12, 12),
          signal.color.withAlpha(glow),
          0,
          0
        );
        renderer.drawUiShape(
          QuadKindEnum::solid,
          point,
          Vec2Props::of(8, 8),
          ColorProps::black().withAlpha(0.7F),
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
          Vec2Props::of(11, 11),
          signal.color.withAlpha(glow),
          ScalarMath::pi / 4.0F,
          0
        );
        if(signal.item)
          renderer.drawUiSprite(
            context.art.sprites().itemIcon(*signal.item),
            point.add(Vec2Props::of(16, -10)),
            14,
            ColorProps::white().withAlpha(glow)
          );
        break;

      case SignalKindEnum::exit:
        renderer.drawUiShape(
          QuadKindEnum::ring,
          point,
          Vec2Props::of(18, 18),
          signal.color.withAlpha(glow),
          0,
          0.25F
        );
        renderer.drawText(
          Localization::text(exit_label),
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
          Vec2Props::of(14, 14),
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
