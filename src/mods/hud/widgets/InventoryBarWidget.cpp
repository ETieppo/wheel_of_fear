#include "mods/hud/widgets/InventoryBarWidget.hpp"
#include "core/items/ItemCatalog.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/render/UiPainter.hpp"
#include "core/theme/Palette.hpp"
#include "mods/hud/KeyLabelCatalog.hpp"
#include "mods/player/components/InventoryComponent.hpp"
#include <format>

namespace wheel_of_fear {
  void InventoryBarWidget::draw(
    Renderer                 &renderer,
    const SpriteSheetProps   &sprites,
    const InventoryComponent &inventory,
    std::uint32_t             phase
  ) {
    constexpr float slot_size = 40;
    constexpr float top       = RenderConfig::ui_height - 14 - slot_size - 14;
    float           cursor_x  = 14;

    for(const ItemEnum item : ItemCatalog::throwables()) {
      const int        count = inventory.count(item);
      const ColorProps color = ItemCatalog::of(item).colorForPhase(phase);

      UiPainter::drawPanel(
        renderer,
        cursor_x,
        top,
        slot_size + 12,
        slot_size + 14,
        count > 0 ? color : Palette::muted_text,
        0.9F
      );

      renderer.drawUiSprite(
        sprites.itemIcon(item),
        Vec2Props::of(cursor_x + 26, top + 24),
        26,
        count > 0 ? color : Palette::muted_text.withAlpha(0.5F)
      );

      renderer.drawText(
        std::format("{}", count),
        cursor_x + 46,
        top + 34,
        14,
        Palette::text,
        TextAlignEnum::right
      );

      const InputActionEnum action = item == ItemEnum::lure_beacon
        ? InputActionEnum::throw_lure
        : InputActionEnum::throw_grenade;

      renderer.drawText(
        KeyLabelCatalog::labelFor(action),
        cursor_x + 7,
        top + 3,
        11,
        color,
        TextAlignEnum::left
      );

      cursor_x += slot_size + 20;
    }

    for(const ItemEnum item : ItemCatalog::generatorParts()) {
      const int count = inventory.count(item);
      if(count == 0) continue;

      renderer.drawUiSprite(
        sprites.itemIcon(item),
        Vec2Props::of(cursor_x + 12, top + 26),
        20,
        ItemCatalog::of(item).color()
      );

      renderer.drawText(
        std::format("x{}", count),
        cursor_x + 24,
        top + 18,
        14,
        Palette::text,
        TextAlignEnum::left
      );

      cursor_x += 54;
    }
  }
} // namespace wheel_of_fear
