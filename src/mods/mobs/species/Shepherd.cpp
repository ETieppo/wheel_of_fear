#include "mods/mobs/species/Shepherd.hpp"

#include "core/time/GameClock.hpp"
#include "mods/art/Art.hpp"
#include "mods/mobs/Mobs.hpp"
#include "mods/noise/Noise.hpp"
#include "mods/player/Player.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {

void Shepherd::think(GameContextDto& context) {
    const float delta = context.clock.deltaSeconds();
    seconds_since_call_ += delta;
    if (!canAct()) return;
    const Vec2 player_position = context.player.position();
    if (intent() == MobIntentEnum::calling) {
        faceToward(player_position);
        if (intentSeconds() > call_seconds) startSearching(position());
        addIntentSeconds(delta);
        return;
    }
    if (canSeePlayerNow() && seconds_since_call_ >= call_interval_seconds) {
        call(context);
        return;
    }
    if (canSeePlayerNow() && distanceToPlayer(context) < preferred_distance) {
        moveAlong(context, position().sub(player_position).normalized(), currentSpeed());
        faceToward(player_position);
        return;
    }
    thinkAsHunter(context);
}

void Shepherd::call(GameContextDto& context) {
    seconds_since_call_ = 0;
    changeIntent(MobIntentEnum::calling, position());
    playSound(context, SoundEnum::shepherd_call, 1.0F, context.world.random().range(0.9F, 1.1F));
    context.noise.emit(position(), call_radius * 0.5F, NoiseSourceEnum::shepherd_call);
    context.mobs.answerShepherd(position(), context.player.position(), call_radius, call_frenzy_seconds);
}

void Shepherd::draw(GameContextDto& context) const {
    const SpriteSheetProps& sprites = context.art.sprites();
    const std::size_t frame = animationFrame(isMoving() ? 8.0F : 2.0F, sprites.shepherd_walk.size());
    drawBody(context, sprites.shepherd_walk[frame], sprites.shepherd_walk_glow[frame], MobLookDto{.glow_color = Color::hex(0xb06bff)});
    if (seconds_since_call_ >= call_ripple_seconds || petrifyAmount() >= 0.5F) return;
    const float progress = seconds_since_call_ / call_ripple_seconds;
    context.renderer.drawGlowSprite(sprites.twirl, position(), Color::hex(0xa060ff).withAlpha(0.4F * (1 - progress)), SpriteDrawDto{.rotation = progress * 3, .scale = 1 + progress * 7});
}

}
