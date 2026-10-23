#include "mods/hud/widgets/VitalsPanel.hpp"

#include <cmath>

#include "core/math/Scalar.hpp"
#include "core/render/UiKit.hpp"
#include "core/theme/Palette.hpp"
#include "mods/player/Player.hpp"

namespace wheel_of_fear {

void VitalsPanel::draw(Renderer& renderer, const Player& player, float real_seconds) {
    UiKit::drawPanel(renderer, 14, 14, 238, 86, Palette::panel_edge, 1);
    renderer.drawText("VITALS", 28, 20, 12, Palette::muted_text, TextAlignEnum::left);
    const int health = player.vitality().health();
    for (int slot = 0; slot < Vitality::max_health; ++slot) {
        const bool is_filled = slot < health;
        const float pulse = is_filled && health == 1 ? 0.6F + 0.4F * std::sin(real_seconds * 9) : 1.0F;
        const Color color = is_filled ? Palette::hud_magenta.withAlpha(pulse) : Color::hex(0x2a1a2a);
        renderer.drawUiShape(QuadKindEnum::solid, Vec2::of(94 + static_cast<float>(slot) * 30, 28), Vec2::of(24, 9), color, -0.18F, 0);
    }

    const Flashlight& flashlight = player.flashlight();
    const FlashlightBattery& battery = flashlight.battery();
    renderer.drawText("CELL", 28, 42, 12, Palette::muted_text, TextAlignEnum::left);
    const bool is_low = battery.level() < 0.2F;
    const float blink_alpha = Scalar::floorModulo(real_seconds, 0.5F) < 0.25F ? 1.0F : 0.35F;
    Color cell_color = flashlight.isUltraviolet() ? Palette::ultraviolet_light : Palette::neon_cyan;
    if (is_low) cell_color = Palette::danger.withAlpha(blink_alpha);
    UiKit::drawSegmentedBar(renderer, 80, 45, 12, 11, 8, battery.level(), cell_color, Color::hex(0x141a26));

    std::string_view mode_label = flashlight.isUltraviolet() ? "ULTRAVIOLET" : "WHITE BEAM";
    if (!flashlight.isEnabled()) mode_label = "LIGHT OFF";
    if (player.isRepairing()) mode_label = "LIGHT IN MOUTH";
    const Color mode_color = flashlight.isUltraviolet() && flashlight.isEnabled() ? Palette::ultraviolet_light : Palette::text;
    renderer.drawText(mode_label, 28, 64, 14, mode_color, TextAlignEnum::left);

    if (battery.isCharging()) {
        const float charge_pulse = 0.5F + 0.5F * std::sin(real_seconds * 6);
        renderer.drawText("CHARGING", 238, 64, 14, Palette::success.withAlpha(0.5F + 0.5F * charge_pulse), TextAlignEnum::right);
    } else if (is_low) {
        renderer.drawText("LOW CELL", 238, 64, 14, Palette::danger.withAlpha(blink_alpha), TextAlignEnum::right);
    }
}

}
