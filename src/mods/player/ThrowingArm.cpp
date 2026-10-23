#include "mods/player/ThrowingArm.hpp"

#include <algorithm>
#include <format>

#include "core/audio/SoundPlayer.hpp"
#include "core/items/ItemCatalog.hpp"
#include "core/theme/Palette.hpp"
#include "mods/hud/Hud.hpp"
#include "mods/player/Inventory.hpp"
#include "mods/throwables/Throwables.hpp"

namespace wheel_of_fear {

void ThrowingArm::tick(float delta_seconds) {
    cooldown_seconds_ = std::max(0.0F, cooldown_seconds_ - delta_seconds);
}

void ThrowingArm::tryThrow(GameContextDto& context, Inventory& inventory, ItemEnum item, Vec2 from, Vec2 target) {
    if (cooldown_seconds_ > 0) return;
    const ThrowableItem& throwable = ItemCatalog::throwable(item);
    if (!inventory.consume(item)) {
        context.hud.announce(Palette::muted_text, std::format("NO {} LEFT", throwable.displayName()));
        return;
    }
    cooldown_seconds_ = cooldown_after_throw_seconds;
    context.throwables.launch(item, from, target);
    context.sound_player.play(SoundEnum::decoy_throw, 0.55F, 0, throwable.throwPitch());
}

}
