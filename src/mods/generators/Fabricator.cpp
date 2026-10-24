#include "mods/generators/Fabricator.hpp"

#include <algorithm>
#include <cmath>
#include <format>

#include "core/audio/SoundPlayer.hpp"
#include "core/items/ItemCatalog.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/Art.hpp"
#include "mods/effects/Effects.hpp"
#include "mods/hud/Hud.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/player/Player.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  namespace {
    constexpr float interaction_distance       = 18;
    constexpr float fabrication_seconds        = 2.6F;
    constexpr float cooldown_after_fabrication = 22;
    constexpr float fabrication_noise_radius   = 260;
    constexpr float noise_interval_seconds     = 0.6F;
    constexpr float front_offset               = 16;
  } // namespace

  Fabricator::Fabricator(
    Vec2        position,
    std::size_t room_index,
    ItemEnum    produced_item
  )
      : Entity(
          position,
          8
        )
      , room_index_(room_index)
      , produced_item_(produced_item) {
  }

  Vec2 Fabricator::frontPoint() const {
    return position().add(Vec2::of(0, front_offset));
  }

  void Fabricator::update(
    GameContextDto &context
  ) {
    const float delta = context.clock.deltaSeconds();
    advanceAnimation(delta);
    cooldown_seconds_            = std::max(0.0F, cooldown_seconds_ - delta);
    const Player &player         = context.player;
    const bool    is_player_near = frontPoint().distance(player.position()) <
                                   interaction_distance &&
                                   player.canBeTargeted();
    if(!is_player_near) {
      interrupt(context);
      return;
    }
    const std::string_view item_name =
      ItemCatalog::of(produced_item_).displayName();
    if(!isReady()) {
      context.hud.showPrompt(
        std::nullopt,
        std::format("FABRICATOR RECHARGING  {:.0f}s", cooldown_seconds_)
      );
      return;
    }
    if(!player.isInteracting()) {
      interrupt(context);
      context.hud.showPrompt(
        InputActionEnum::interact,
        std::format("hold to fabricate a {}", item_name)
      );
      return;
    }
    work(context);
  }

  void Fabricator::work(
    GameContextDto &context
  ) {
    const float delta = context.clock.deltaSeconds();
    if(
      work_voice_ == no_voice || !context.sound_player.isPlaying(work_voice_)
    ) {
      work_voice_ = context.sound_player.playAtTracked(
        SoundEnum::machine_work,
        0.6F,
        context.player.position(),
        position(),
        1
      );
    }
    work_progress_ += delta;
    noise_timer_ -= delta;
    if(noise_timer_ <= 0) {
      noise_timer_ = noise_interval_seconds;
      context.noise.emit(
        position(),
        fabrication_noise_radius,
        NoiseSourceEnum::fabricating
      );
    }
    const std::string_view item_name =
      ItemCatalog::of(produced_item_).displayName();
    context.hud.showPrompt(
      std::nullopt,
      std::format(
        "FABRICATING {}  {:.0f}%",
        item_name,
        work_progress_ / fabrication_seconds * 100
      )
    );
    if(work_progress_ >= fabrication_seconds) deliver(context);
  }

  void Fabricator::deliver(
    GameContextDto &context
  ) {
    interrupt(context);
    const Item &item = ItemCatalog::of(produced_item_);
    if(!context.player.inventory().receive(produced_item_)) {
      context.hud.announce(
        Palette::muted_text,
        std::format("YOU CAN'T CARRY MORE {}", item.displayName())
      );
      return;
    }
    cooldown_seconds_ = cooldown_after_fabrication;
    context.effects.emitSparks(context.world, position(), 10, item.color());
    context.sound_player.play(SoundEnum::part_pickup, 0.8F, 0, 1);
    context.hud.announce(
      item.color(),
      std::format("FABRICATED  {}", item.displayName())
    );
  }

  void Fabricator::interrupt(
    GameContextDto &context
  ) {
    work_progress_ = 0;
    context.sound_player.stop(work_voice_);
    work_voice_ = no_voice;
  }

  void Fabricator::draw(
    GameContextDto &context
  ) const {
    Renderer               &renderer     = context.renderer;
    const SpriteSheetProps &sprites      = context.art.sprites();
    const float             real_seconds = context.clock.realSeconds();
    const Vec2              center       = position();
    const Color item_color = ItemCatalog::of(produced_item_).color();
    renderer.drawShadowDisc(
      center.add(Vec2::of(1, 4)),
      11,
      Color::black().withAlpha(0.5F),
      1.3F
    );
    renderer.drawSprite(sprites.fabricator, center);
    const Color screen_color = isReady() ? item_color : Palette::neon_red;
    const float flicker = 0.75F + 0.25F * std::sin(real_seconds * 7 + center.x);
    renderer.drawGlowSprite(
      sprites.fabricator_glow,
      center,
      screen_color.withAlpha(flicker)
    );
    const float hover = std::sin(real_seconds * 2.5F + center.x) * 1.5F;
    renderer.drawGlowSprite(
      sprites.itemIcon(produced_item_),
      center.add(Vec2::of(0, -18 + hover)),
      item_color.withAlpha(isReady() ? 0.85F : 0.25F),
      SpriteDrawDto {.scale = 0.9F}
    );
    if(work_progress_ > 0)
      renderer.drawGlowRectangle(
        center.add(Vec2::of(0, 12)),
        Vec2::of(18 * work_progress_ / fabrication_seconds, 2),
        screen_color.withAlpha(0.9F)
      );
  }
} // namespace wheel_of_fear
