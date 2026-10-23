#pragma once

#include "core/audio/enums/SoundEnum.hpp"
#include "core/entity/Entity.hpp"
#include "core/render/AtlasRegionProps.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/mobs/MobMemoryProps.hpp"
#include "mods/mobs/MobTraitsProps.hpp"
#include "mods/mobs/dtos/MobLookDto.hpp"
#include "mods/mobs/enums/MobIntentEnum.hpp"
#include "mods/mobs/enums/MobKindEnum.hpp"

namespace wheel_of_fear {

class Mob : public Entity {
public:
    Mob(Vec2 position, const MobTraitsProps& traits);

    virtual MobKindEnum kind() const = 0;
    virtual const MobTraitsProps& traits() const = 0;
    virtual void think(GameContextDto& context) { thinkAsHunter(context); }
    virtual void onContact(GameContextDto& context);
    virtual void onLightReleased(GameContextDto&) {}
    virtual void emitLights(GameContextDto&) {}
    virtual void draw(GameContextDto& context) const = 0;
    virtual bool isDisguised() const { return false; }
    virtual void ignite(GameContextDto& context);
    virtual void catchBlast(GameContextDto& context);
    virtual void onGeneratorOnline(GameContextDto&) {}

    void update(GameContextDto& context);
    void perceive(GameContextDto& context, bool can_notice);
    void witnessHiding(std::size_t locker_index) { memory_.known_locker_index = locker_index; }
    void hearShepherd(Vec2 target);
    void playSound(GameContextDto& context, SoundEnum sound, float volume, float pitch) const;
    void advanceBurning(GameContextDto& context);
    void advanceHugAndCrumble(GameContextDto& context);

    MobIntentEnum intent() const { return intent_; }
    const MobMemoryProps& memory() const { return memory_; }
    bool isFrozen() const { return is_frozen_; }
    void setFrozen(bool is_frozen) { is_frozen_ = is_frozen; }
    bool isBeingHugged() const { return is_being_hugged_; }
    void setBeingHugged(bool is_being_hugged) { is_being_hugged_ = is_being_hugged; }
    bool isCrumbling() const { return crumble_seconds_ > 0; }
    bool isBurning() const { return burn_seconds_ > 0; }
    bool isBusy() const { return !isAlive() || is_being_hugged_ || isCrumbling() || isBurning(); }
    bool canAct() const { return !isBusy() && !is_frozen_; }
    bool isHunting() const;
    float incidentCooldown() const { return incident_cooldown_; }
    void setIncidentCooldown(float seconds) { incident_cooldown_ = seconds; }
    float frenzySeconds() const { return frenzy_seconds_; }
    void frenzy(float seconds);
    void calmFrenzy(float delta_seconds);
    float petrifyAmount() const { return petrify_amount_; }
    void setVisibility(float visibility) { visibility_ = visibility; }
    float presence() const { return opacity_ * visibility_; }
    float distanceToPlayer(const GameContextDto& context) const;

protected:
    void thinkAsHunter(GameContextDto& context);
    void changeIntent(MobIntentEnum intent, Vec2 destination);
    bool touchesPlayer(const GameContextDto& context, float reach) const;
    void startSearching(Vec2 anchor);
    bool navigateTo(GameContextDto& context, Vec2 destination, float speed);
    bool moveAlong(GameContextDto& context, Vec2 direction, float speed);
    void strikeAndRetreat(GameContextDto& context, float retreat_seconds);
    void strikePlayer(GameContextDto& context);
    void exposeHiddenPlayer(GameContextDto& context);
    void vocalize(GameContextDto& context, float chance);
    void drawBody(GameContextDto& context, const AtlasRegionProps& body, const AtlasRegionProps& glow, const MobLookDto& look) const;
    float currentSpeed() const;

    float intentSeconds() const { return intent_seconds_; }
    void addIntentSeconds(float seconds) { intent_seconds_ += seconds; }
    Vec2 destination() const { return destination_; }
    bool canSeePlayerNow() const { return can_see_player_; }
    float vocalCooldown() const { return vocal_cooldown_; }
    void setVocalCooldown(float seconds) { vocal_cooldown_ = seconds; }
    void setRetreatSeconds(float seconds) { retreat_seconds_ = seconds; }
    float opacity() const { return opacity_; }
    void setOpacity(float opacity) { opacity_ = opacity; }
    float visibility() const { return visibility_; }

private:
    bool canSeePlayer(const GameContextDto& context) const;
    void chooseIntent(GameContextDto& context);
    void pursueIntent(GameContextDto& context);
    void trackProgress(GameContextDto& context);
    void patrol(GameContextDto& context);
    void investigate(GameContextDto& context);
    void search(GameContextDto& context);
    void chase(GameContextDto& context);
    void lurk(GameContextDto& context);
    void lookAround(GameContextDto& context);
    void fleeFromPlayer(GameContextDto& context);
    void playMovementSound(GameContextDto& context);
    Vec2 trembleOffset(float world_seconds) const;

    MobIntentEnum intent_ = MobIntentEnum::patrolling;
    float intent_seconds_ = 0;
    Vec2 destination_;
    std::optional<std::size_t> patrol_room_;
    float linger_seconds_ = 0;
    Vec2 search_anchor_{};
    MobMemoryProps memory_{};
    float vocal_cooldown_ = 0;
    float movement_sound_cooldown_ = 0;
    float incident_cooldown_ = 0;
    float strike_cooldown_ = 0;
    float frenzy_seconds_ = 0;
    float retreat_seconds_ = 0;
    float petrify_amount_ = 0;
    float crumble_seconds_ = 0;
    float burn_seconds_ = 0;
    float opacity_ = 1;
    float visibility_ = 0;
    bool is_frozen_ = false;
    bool is_being_hugged_ = false;
    bool can_see_player_ = false;
};

}
