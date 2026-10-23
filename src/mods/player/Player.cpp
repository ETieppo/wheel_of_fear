#include "mods/player/Player.hpp"

#include "core/audio/SoundPlayer.hpp"
#include "core/input/Input.hpp"
#include "core/math/Scalar.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/Art.hpp"
#include "mods/camera/Camera.hpp"
#include "mods/hud/Hud.hpp"
#include "mods/lighting/Lighting.hpp"
#include "mods/player/PlayerAnimator.hpp"
#include "mods/tracker/Tracker.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {

Player::Player() : Entity(Vec2{}, collision_half_size) {
    respawn(Vec2{});
}

void Player::respawn(Vec2 position) {
    respawnAt(position);
    aim_direction_ = Vec2::of(1, 0);
    activity_ = PlayerActivityEnum::exploring;
    repair_target_.reset();
    is_interacting_ = false;
    flashlight_.reset();
    inventory_.restock();
    vitality_.restore();
    hiding_.reset();
    movement_.reset();
    throwing_arm_.reset();
}

float Player::sightRangeForMobs() const {
    if (!canBeTargeted()) return 0;
    if (isRepairing()) return 190;
    if (flashlight_.isOn()) return flashlight_.sightRangeForMobs();
    if (movement_.isRunning()) return 170;
    if (isMoving()) return 105;
    return 60;
}

FlashlightGripDto Player::grip() const {
    return FlashlightGripDto{position(), aim_direction_, repair_target_};
}

void Player::update(GameContextDto& context) {
    hiding_.clearHideEvent();
    if (context.world.map().width() > 0 && !isDead()) emitLights(context);
    if (!context.clock.isRunning()) return;
    const float delta = context.clock.deltaSeconds();
    advanceAnimation(delta);
    vitality_.tick(delta);
    throwing_arm_.tick(delta);
    if (isDead()) return;

    is_interacting_ = context.input.isHeld(InputActionEnum::interact);
    if (!is_interacting_) repair_target_.reset();
    const PlayerActivityEnum previous_activity = activity_;
    activity_ = PlayerActivityEnum::exploring;
    if (repair_target_) activity_ = PlayerActivityEnum::repairing;
    if (hiding_.isHidden()) activity_ = PlayerActivityEnum::hiding;

    switch (activity_) {
    case PlayerActivityEnum::hiding:
        hiding_.updateWhileHidden(context, *this);
        if (!hiding_.isHidden()) activity_ = PlayerActivityEnum::exploring;
        break;
    case PlayerActivityEnum::repairing: faceRepairTarget(); break;
    case PlayerActivityEnum::exploring:
    case PlayerActivityEnum::hugging: explore(context, previous_activity); break;
    }
    updateFlashlight(context);
}

void Player::faceRepairTarget() {
    aim_direction_ = repair_target_->sub(position()).normalized();
    setHeading(aim_direction_.angle());
    setMoving(false);
}

void Player::explore(GameContextDto& context, PlayerActivityEnum previous_activity) {
    aimAtMouse(context);
    activity_ = hug_controller_.update(context, *this, aim_direction_, previous_activity);
    if (activity_ != PlayerActivityEnum::hugging) movement_.update(context, *this);
    handleExplorationActions(context);
}

void Player::aimAtMouse(GameContextDto& context) {
    const Vec2 toward_mouse = context.camera.mouseWorldPosition(context.input).sub(position());
    if (toward_mouse.length() > 2) aim_direction_ = toward_mouse.normalized();
    turnToward(aim_direction_.angle(), 1.0F / 30.0F, aim_turn_rate);
}

void Player::handleExplorationActions(GameContextDto& context) {
    const Input& input = context.input;
    if (input.wasPressed(InputActionEnum::switch_light_mode)) flashlight_.cycleMode(context.sound_player);
    const Vec2 target = context.camera.mouseWorldPosition(input);
    const Vec2 lamp_tip = Flashlight::lampTip(position(), aim_direction_);
    if (input.wasPressed(InputActionEnum::throw_lure)) throwing_arm_.tryThrow(context, inventory_, ItemEnum::lure_beacon, lamp_tip, target);
    if (input.wasPressed(InputActionEnum::throw_grenade)) throwing_arm_.tryThrow(context, inventory_, ItemEnum::neon_grenade, lamp_tip, target);
    const auto locker_index = hiding_.nearestLocker(context.world, position());
    if (!locker_index) return;
    context.hud.showPrompt(InputActionEnum::interact, "hide in the locker");
    if (!input.wasPressed(InputActionEnum::interact)) return;
    repair_target_.reset();
    hiding_.enter(context, *this, *locker_index);
    activity_ = PlayerActivityEnum::hiding;
}

void Player::updateFlashlight(GameContextDto& context) {
    FlashlightConditionsDto conditions;
    conditions.delta_seconds = context.clock.deltaSeconds();
    conditions.is_held_in_mouth = isRepairing();
    conditions.is_tracker_open = context.tracker.isOpen();
    conditions.is_holder_hidden = isHidden();
    conditions.is_in_burning_light = context.world.isBurningLightAt(position());
    flashlight_.update(conditions, context.world.random(), context.sound_player);
}

void Player::beginRepair(Vec2 target) {
    if (isHidden() || isDead()) return;
    repair_target_ = target;
    activity_ = PlayerActivityEnum::repairing;
}

void Player::takeHit(GameContextDto& context, Vec2 source) {
    if (isHidden() || !vitality_.absorbHit()) return;
    movement_.push(position().sub(source).normalized().scale(220));
    repair_target_.reset();
    context.world.shake(1);
    context.sound_player.play(SoundEnum::player_hurt, 0.9F, 0, context.world.random().range(0.9F, 1.1F));
    if (vitality_.isDead()) context.sound_player.play(SoundEnum::player_death, 1.0F, 0, 1);
}

void Player::dragOutOfLocker(GameContextDto& context) {
    hiding_.leave(context, *this);
    activity_ = PlayerActivityEnum::exploring;
    vitality_.dropGuard();
    context.world.shake(0.8F);
}

void Player::emitLights(GameContextDto& context) const {
    if (isHidden()) return;
    LightProps aura;
    aura.position = position();
    aura.radius = aura_radius;
    aura.intensity = 0.55F;
    aura.color = Color::hex(0x8fa6d8);
    aura.casts_shadows = false;
    context.lighting.add(aura);
    flashlight_.emitLights(context.lighting, grip());
}

void Player::draw(GameContextDto& context) const {
    if (context.world.map().width() == 0 || isHidden()) return;
    const float real_seconds = context.clock.realSeconds();
    const bool is_blinking = vitality_.isInvulnerable() && vitality_.hurtFlash() > 0 && !isDead() && Scalar::floorModulo(real_seconds, 0.16F) < 0.08F;
    if (is_blinking) return;
    Renderer& renderer = context.renderer;
    const SpriteSheetProps& sprites = context.art.sprites();
    const Color tint = isDead() ? Color::hex(0x6a6a72) : Color::white();
    const float rotation = isDead() ? heading() + 1.4F : heading();
    renderer.drawShadowDisc(position().add(Vec2::of(1.5F, 2.5F)), 8, Color::black().withAlpha(0.55F), 1.4F);
    const PlayerPoseDto pose = PlayerAnimator::currentPose(sprites, activity_, isMoving(), movement_.strideDistance(), real_seconds);
    renderer.drawSprite(pose.body, position(), SpriteDrawDto{.rotation = rotation, .color = tint});
    if (!isDead()) renderer.drawGlowSprite(pose.glow, position(), Palette::neon_cyan.withAlpha(0.95F), SpriteDrawDto{.rotation = rotation});
    flashlight_.drawBeam(renderer, sprites, grip());
}

}
