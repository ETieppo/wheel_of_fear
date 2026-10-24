#include "mods/hud/Hud.hpp"

#include "core/input/Input.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/Renderer.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/Art.hpp"
#include "mods/generators/Generators.hpp"
#include "mods/hud/widgets/Crosshair.hpp"
#include "mods/hud/widgets/HidingOverlay.hpp"
#include "mods/hud/widgets/InventoryBar.hpp"
#include "mods/hud/widgets/ObjectivesPanel.hpp"
#include "mods/hud/widgets/SkillCheckDial.hpp"
#include "mods/hud/widgets/VitalsPanel.hpp"
#include "mods/player/Player.hpp"
#include "mods/tracker/Tracker.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  void Hud::beginFrame(
    float real_delta_seconds
  ) {
    prompt_.clear();
    messages_.age(real_delta_seconds);
  }

  void Hud::draw(
    GameContextDto &context
  ) const {
    if(!is_visible_) return;
    Renderer     &renderer     = context.renderer;
    const Player &player       = context.player;
    const float   real_seconds = context.clock.realSeconds();
    const float   hurt_flash   = player.vitality().hurtFlash();
    if(hurt_flash > 0)
      renderer.drawUiRectangle(
        -400,
        -400,
        RenderConfig::ui_width + 800,
        RenderConfig::ui_height + 800,
        Palette::danger.withAlpha(hurt_flash * 0.2F)
      );
    if(context.tracker.openAmount() > 0.6F) return;
    if(player.isHidden())
      HidingOverlay::draw(renderer, player.hiding(), real_seconds);
    VitalsPanel::draw(renderer, player, real_seconds);
    ObjectivesPanel::draw(renderer, context.world.phase(), context.generators);
    InventoryBar::draw(renderer, context.art.sprites(), player.inventory());
    messages_.draw(renderer);
    if(const Generator* generator = context.generators.activeRepair())
      SkillCheckDial::draw(renderer, generator->skillCheck(), real_seconds);
    prompt_.draw(renderer);
    if(player.isHidden()) return;
    const Color crosshair_color = player.flashlight().isUltraviolet()
                                  ? Palette::ultraviolet_light
                                  : Palette::neon_cyan;
    Crosshair::draw(
      renderer,
      context.input.mouseInUi(),
      crosshair_color,
      real_seconds
    );
  }
} // namespace wheel_of_fear
