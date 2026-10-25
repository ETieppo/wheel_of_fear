#include "mods/hud/HudSystem.hpp"
#include "core/input/InputController.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/Renderer.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/generators/GeneratorSystem.hpp"
#include "mods/hud/widgets/CrosshairWidget.hpp"
#include "mods/hud/widgets/HidingOverlayWidget.hpp"
#include "mods/hud/widgets/InventoryBarWidget.hpp"
#include "mods/hud/widgets/ObjectivesPanelWidget.hpp"
#include "mods/hud/widgets/SkillCheckDialWidget.hpp"
#include "mods/hud/widgets/VitalsPanelWidget.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/tracker/TrackerSystem.hpp"
#include "mods/world/WorldSystem.hpp"

namespace wheel_of_fear {
  void HudSystem::beginFrame(float real_delta_seconds) {
    prompt_.clear();
    messages_.age(real_delta_seconds);
  }

  void HudSystem::draw(GameContextDto &context) const {
    if(!is_visible_) return;

    Renderer           &renderer     = context.renderer;
    const PlayerEntity &player       = context.player;
    const float         real_seconds = context.clock.realSeconds();
    const float         hurt_flash   = player.vitality().hurtFlash();

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
      HidingOverlayWidget::draw(renderer, player.hiding(), real_seconds);

    VitalsPanelWidget::draw(renderer, player, real_seconds);
    ObjectivesPanelWidget::draw(
      renderer,
      context.world.phase(),
      context.generators
    );

    InventoryBarWidget::draw(
      renderer,
      context.art.sprites(),
      player.inventory(),
      context.world.phase()
    );

    messages_.draw(renderer);
    if(const GeneratorEntity* generator = context.generators.activeRepair())
      SkillCheckDialWidget::draw(
        renderer,
        generator->skillCheck(),
        real_seconds
      );

    prompt_.draw(renderer);
    if(player.isHidden()) return;

    const ColorProps crosshair_color = player.flashlight().isUltraviolet()
      ? Palette::ultraviolet_light
      : Palette::neon_cyan;

    CrosshairWidget::draw(
      renderer,
      context.input.mouseInUi(),
      crosshair_color,
      real_seconds
    );
  }
} // namespace wheel_of_fear
