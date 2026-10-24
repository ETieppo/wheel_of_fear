#include "mods/generators/ExitDoorEntity.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/hud/HudSystem.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>
#include <format>

namespace wheel_of_fear {
  namespace {
    constexpr float      opening_speed      = 0.5F;
    constexpr float      slide_distance     = 15;
    constexpr float      entry_radius       = 14;
    constexpr ColorProps locked_light_color = ColorProps::hex(0xff2a4a);
  } // namespace

  void ExitDoorEntity::reset(Vec2Props doorway_position) {
    respawnAt(doorway_position);
    is_unlocked_       = false;
    open_amount_       = 0;
    has_player_exited_ = false;
  }

  void ExitDoorEntity::unlock(GameContextDto &context) {
    if(is_unlocked_) return;

    is_unlocked_ = true;
    context.sound_player.play(SoundEnum::door_open, 0.9F, 0, 1);
    context.world.shake(0.6F);
    context.world.powerRoom(context.world.exitRoomIndex(), true);
    context.hud.announce(
      Palette::success,
      "ALL GENERATORS ONLINE  -  THE EXIT IS OPEN"
    );
  }

  void ExitDoorEntity::update(
    GameContextDto &context,
    std::size_t     online_count,
    std::size_t     generator_count
  ) {
    const Vec2Props light_position = position().add(Vec2Props::of(0, 12));
    const float     player_distance
      = context.player.position().distance(position());

    LightProps light;
    light.position = light_position;

    if(!is_unlocked_) {
      const bool is_blinking
        = ScalarMath::floorModulo(context.world.elapsedSeconds(), 1.2F) < 0.15F;

      light.radius        = 46;
      light.intensity     = is_blinking ? 0.8F : 0.15F;
      light.color         = locked_light_color;
      light.casts_shadows = false;
      context.lighting.add(light);

      if(player_distance < 44)
        context.hud.showPrompt(
          std::nullopt,
          std::format(
            "EXIT SEALED  -  {}/{} GENERATORS ONLINE",
            online_count,
            generator_count
          )
        );

      return;
    }

    open_amount_ = std::min(
      1.0F,
      open_amount_ + (context.clock.deltaSeconds() * opening_speed)
    );

    light.radius    = 80;
    light.intensity = 0.9F;
    light.color     = Palette::success;
    light.flicker   = 0.1F;
    context.lighting.add(light);

    if(
      open_amount_
      > 0.6F
      && context.player.position().distance(entryPoint())
      < entry_radius
    )
      has_player_exited_ = true;

    if(player_distance < 64)
      context.hud.showPrompt(std::nullopt, "STEP INTO THE EXIT");
  }

  void ExitDoorEntity::draw(GameContextDto &context) const {
    Renderer               &renderer = context.renderer;
    const SpriteSheetProps &sprites  = context.art.sprites();
    const float             slide    = open_amount_ * slide_distance;
    const Vec2Props         center   = position();

    renderer.drawSprite(sprites.exit_frame, center.add(Vec2Props::of(0, -4)));
    renderer.drawSprite(
      sprites.blast_door_panel,
      center.add(Vec2Props::of(-12 - slide, -2))
    );

    renderer.drawSprite(
      sprites.blast_door_panel,
      center.add(Vec2Props::of(12 + slide, -2)),
      SpriteDrawDto {.rotation = ScalarMath::pi}
    );

    const ColorProps status
      = is_unlocked_ ? Palette::success : locked_light_color;

    renderer.drawGlowSprite(
      sprites.exit_frame_glow,
      center.add(Vec2Props::of(0, -4)),
      status
    );

    if(is_unlocked_)
      renderer.drawGlowDisc(
        center.add(Vec2Props::of(0, 2)),
        12 * open_amount_,
        Palette::success.withAlpha(0.4F),
        1.2F
      );
  }
} // namespace wheel_of_fear
