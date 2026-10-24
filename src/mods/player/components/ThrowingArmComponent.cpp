#include "mods/player/components/ThrowingArmComponent.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/items/ItemCatalog.hpp"
#include "core/theme/Palette.hpp"
#include "mods/hud/HudSystem.hpp"
#include "mods/player/components/InventoryComponent.hpp"
#include "mods/throwables/ThrowableSystem.hpp"
#include <algorithm>
#include <format>

namespace wheel_of_fear {
  void ThrowingArmComponent::tick(float delta_seconds) {
    cooldown_seconds_ = std::max(0.0F, cooldown_seconds_ - delta_seconds);
  }

  void ThrowingArmComponent::tryThrow(
    GameContextDto     &context,
    InventoryComponent &inventory,
    ItemEnum            item,
    Vec2Props           from,
    Vec2Props           target
  ) {
    if(cooldown_seconds_ > 0) return;
    const ThrowableItem &throwable = ItemCatalog::throwable(item);

    if(!inventory.consume(item)) {
      context.hud.announce(
        Palette::muted_text,
        std::format("NO {} LEFT", throwable.displayName())
      );

      return;
    }

    cooldown_seconds_ = cooldown_after_throw_seconds;
    context.throwables.launch(item, from, target);
    context.sound_player
      .play(SoundEnum::decoy_throw, 0.55F, 0, throwable.throwPitch());
  }
} // namespace wheel_of_fear
