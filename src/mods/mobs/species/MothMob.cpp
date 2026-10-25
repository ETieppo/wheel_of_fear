#include "mods/mobs/species/MothMob.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/text/Localization.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/hud/HudSystem.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>
#include <cmath>
#include <string>

namespace wheel_of_fear {
  namespace {
    constexpr LocalizedTextProps moth_chose_message {
      "A LANTERN MOTH CHOSE YOU. ITS GLOW HOLDS THEM STILL.",
      "UMA MARIPOSA-LANTERNA TE ESCOLHEU. SEU BRILHO OS MANTÉM PARADOS.",
    };
    constexpr LocalizedTextProps moth_left_message {
      "THE LANTERN MOTH DRIFTS AWAY INTO THE DARK",
      "A MARIPOSA-LANTERNA SE PERDE NO ESCURO",
    };
  } // namespace

  Vec2Props MothMob::flutter() const {
    return Vec2Props::of(
      std::sin(animationSeconds() * 2.3F) * 6,
      std::sin(animationSeconds() * 3.1F) * 4
    );
  }

  Vec2Props MothMob::lightSource(
    const TileMapModel &map,
    Vec2Props           toward
  ) const {
    const Vec2Props offset      = toward.sub(position());
    const float     length      = offset.length();
    const auto      probe_count = static_cast<int>(length / light_probe_step);

    for(int probe_index = 0; probe_index < probe_count; ++probe_index) {
      const float along = static_cast<float>(probe_index) * light_probe_step;

      const Vec2Props probe = position().add(offset.scale(along / length));
      if(!map.isSolidAt(probe)) return probe;
    }

    return toward;
  }

  void MothMob::emitLights(GameContextDto &context) {
    LightProps light;

    light.position
      = lightSource(context.world.map(), context.player.position());
    light.color = light_color;

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
        light.radius    = companion_light_radius * appearance().opacity();
        light.intensity = 0.6F;
        light.flicker   = 0.3F;
        break;
    }

    context.lighting.add(light);
  }

  void MothMob::think(GameContextDto &context) {
    if(!home_) home_ = position();

    switch(stance_) {
      case MothStanceEnum::waiting:   wait(context); break;
      case MothStanceEnum::following: follow(context); break;
      case MothStanceEnum::departing: depart(context); break;
    }
  }

  void MothMob::wait(GameContextDto &context) {
    if(!home_) return;

    setPosition(home_->add(flutter()));
    if(distanceToPlayer(context) >= befriend_distance) return;

    stance_ = MothStanceEnum::following;
    voice().playSound(context, SoundEnum::part_pickup, 0.5F, 1.4F);

    context.hud.announce(
      Palette::neon_amber,
      std::string(Localization::text(moth_chose_message))
    );
  }

  void MothMob::follow(GameContextDto &context) {
    const PlayerEntity &player = context.player;
    const Vec2Props     perch
      = player.position().add(Vec2Props::of(-12, -14)).add(flutter());

    setPosition(
      position().lerp(perch, std::min(1.0F, context.clock.deltaSeconds() * 3))
    );

    faceToward(player.position().add(player.aimDirection().scale(40)));
    if(voice().vocalCooldown() > 0) return;

    RandomGenerator &random = context.world.random();
    voice().setVocalCooldown(random.range(2, 4));
    voice().playSound(
      context,
      SoundEnum::moth_flutter,
      0.4F,
      random.range(0.9F, 1.2F)
    );
  }

  void MothMob::depart(GameContextDto &context) {
    const float delta = context.clock.deltaSeconds();

    setPosition(position().add(Vec2Props::of(10, -40).scale(delta)));
    departure_seconds_ -= delta;

    appearance().setOpacity(
      std::max(0.0F, departure_seconds_ / departure_duration)
    );
    if(departure_seconds_ <= 0) setAlive(false);
  }

  void MothMob::onGeneratorOnline(GameContextDto &context) {
    if(stance_ != MothStanceEnum::following) return;

    stance_            = MothStanceEnum::departing;
    departure_seconds_ = departure_duration;
    context.sound_player.play(SoundEnum::moth_flutter, 0.7F, 0, 0.8F);
    context.hud.announce(
      Palette::warm_light,
      std::string(Localization::text(moth_left_message))
    );
  }

  void MothMob::draw(GameContextDto &context) const {
    const SpriteSheetProps &sprites  = context.art.sprites();
    Renderer               &renderer = context.renderer;
    const std::size_t frame = animationFrame(14, sprites.moth_flutter.size());

    renderer.drawSprite(
      sprites.moth_flutter[frame],
      position(),
      SpriteDrawDto {
        .rotation = heading(),
        .color    = ColorProps::white().withAlpha(appearance().opacity()),
        .scale    = 0.9F,
      }
    );

    renderer.drawGlowSprite(
      sprites.moth_flutter_glow[frame],
      position(),
      light_color.withAlpha(0.9F * appearance().opacity()),
      SpriteDrawDto {.rotation = heading(), .scale = 0.9F}
    );

    renderer.drawGlowSprite(
      sprites.light[2],
      position(),
      light_color.withAlpha(0.45F * appearance().opacity()),
      SpriteDrawDto {.scale = 1.1F}
    );
  }
} // namespace wheel_of_fear
