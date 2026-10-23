#include "mods/mobs/species/Moth.hpp"

#include <algorithm>
#include <cmath>

#include "core/audio/SoundPlayer.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/Art.hpp"
#include "mods/hud/Hud.hpp"
#include "mods/lighting/Lighting.hpp"
#include "mods/player/Player.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {
  Vec2 Moth::flutter() const {
    return Vec2::of(
      std::sin(animationSeconds() * 2.3F) * 6,
      std::sin(animationSeconds() * 3.1F) * 4
    );
  }

  void Moth::emitLights(GameContextDto &context) {
    LightProps light;
    light.position = position();
    light.color    = light_color;
    switch(stance_) {
      case MothStanceEnum::waiting:
        light.radius    = 44;
        light.intensity = 0.6F;
        light.flicker   = 0.2F;
        break;
      case MothStanceEnum::following:
        light.radius            = companion_light_radius;
        light.intensity         = 1.2F;
        light.flicker           = 0.12F;
        light.freezes_creatures = true;
        break;
      case MothStanceEnum::departing:
        light.radius    = companion_light_radius * opacity();
        light.intensity = 0.6F;
        light.flicker   = 0.3F;
        break;
    }
    context.lighting.add(light);
  }

  void Moth::think(GameContextDto &context) {
    if(!home_) home_ = position();
    switch(stance_) {
      case MothStanceEnum::waiting:   wait(context); break;
      case MothStanceEnum::following: follow(context); break;
      case MothStanceEnum::departing: depart(context); break;
    }
  }

  void Moth::wait(GameContextDto &context) {
    setPosition(home_->add(flutter()));
    if(distanceToPlayer(context) >= befriend_distance) return;
    stance_ = MothStanceEnum::following;
    playSound(context, SoundEnum::part_pickup, 0.5F, 1.4F);
    context.hud.announce(
      Palette::neon_amber,
      "A LANTERN MOTH CHOSE YOU. ITS GLOW HOLDS THEM STILL."
    );
  }

  void Moth::follow(GameContextDto &context) {
    const Player &player = context.player;
    const Vec2 perch = player.position().add(Vec2::of(-12, -14)).add(flutter());
    setPosition(
      position().lerp(perch, std::min(1.0F, context.clock.deltaSeconds() * 3))
    );
    faceToward(player.position().add(player.aimDirection().scale(40)));
    if(vocalCooldown() > 0) return;
    RandomGenerator &random = context.world.random();
    setVocalCooldown(random.range(2, 4));
    playSound(context, SoundEnum::moth_flutter, 0.4F, random.range(0.9F, 1.2F));
  }

  void Moth::depart(GameContextDto &context) {
    const float delta = context.clock.deltaSeconds();
    setPosition(position().add(Vec2::of(10, -40).scale(delta)));
    departure_seconds_ -= delta;
    setOpacity(std::max(0.0F, departure_seconds_ / departure_duration));
    if(departure_seconds_ <= 0) setAlive(false);
  }

  void Moth::onGeneratorOnline(GameContextDto &context) {
    if(stance_ != MothStanceEnum::following) return;
    stance_            = MothStanceEnum::departing;
    departure_seconds_ = departure_duration;
    context.sound_player.play(SoundEnum::moth_flutter, 0.7F, 0, 0.8F);
    context.hud.announce(
      Palette::warm_light, "THE LANTERN MOTH DRIFTS AWAY INTO THE DARK"
    );
  }

  void Moth::draw(GameContextDto &context) const {
    const SpriteSheetProps &sprites  = context.art.sprites();
    Renderer               &renderer = context.renderer;
    const std::size_t frame = animationFrame(14, sprites.moth_flutter.size());
    renderer.drawSprite(
      sprites.moth_flutter[frame],
      position(),
      SpriteDrawDto {
        .rotation = heading(),
        .color    = Color::white().withAlpha(opacity()),
        .scale    = 0.9F
      }
    );
    renderer.drawGlowSprite(
      sprites.moth_flutter_glow[frame],
      position(),
      light_color.withAlpha(0.9F * opacity()),
      SpriteDrawDto {.rotation = heading(), .scale = 0.9F}
    );
    renderer.drawGlowSprite(
      sprites.light[2],
      position(),
      light_color.withAlpha(0.45F * opacity()),
      SpriteDrawDto {.scale = 1.1F}
    );
  }
} // namespace wheel_of_fear
