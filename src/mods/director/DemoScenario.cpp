#include "mods/director/DemoScenario.hpp"

#include <algorithm>
#include <array>
#include <format>
#include <optional>
#include <string>

#include <SDL3/SDL_log.h>

#include "core/input/Input.hpp"
#include "core/math/Scalar.hpp"
#include "core/render/RenderConfig.hpp"
#include "core/time/GameClock.hpp"
#include "mods/camera/Camera.hpp"
#include "mods/director/DemoModeParser.hpp"
#include "mods/generators/Generators.hpp"
#include "mods/mobs/Mobs.hpp"
#include "mods/mobs/species/Moth.hpp"
#include "mods/player/Player.hpp"
#include "mods/throwables/Throwables.hpp"
#include "mods/tracker/Tracker.hpp"
#include "mods/world/World.hpp"

namespace wheel_of_fear {

namespace {

void logLine(const std::string& line) {
    SDL_Log("info(demo): %s", line.c_str());
}

std::string optionalIndex(std::optional<std::size_t> value) {
    return value ? std::to_string(*value) : std::string("null");
}

std::string_view kindName(MobKindEnum kind) {
    switch (kind) {
    case MobKindEnum::crawler: return "crawler";
    case MobKindEnum::bloater: return "bloater";
    case MobKindEnum::melter: return "melter";
    case MobKindEnum::shepherd: return "shepherd";
    case MobKindEnum::shade: return "shade";
    case MobKindEnum::mimic: return "mimic";
    case MobKindEnum::moth: return "moth";
    }
    return "";
}

std::string_view stateName(GeneratorStateEnum state) {
    switch (state) {
    case GeneratorStateEnum::offline: return "offline";
    case GeneratorStateEnum::charging: return "charging";
    case GeneratorStateEnum::jammed: return "jammed";
    case GeneratorStateEnum::broken: return "broken";
    case GeneratorStateEnum::online: return "online";
    }
    return "";
}

std::string_view resultName(SkillCheckResultEnum result) {
    switch (result) {
    case SkillCheckResultEnum::none: return "none";
    case SkillCheckResultEnum::perfect: return "perfect";
    case SkillCheckResultEnum::good: return "good";
    case SkillCheckResultEnum::failed: return "failed";
    }
    return "";
}

}

DemoScenario::DemoScenario() : mode_(DemoModeParser::fromEnvironment()) {}

std::uint32_t DemoScenario::startingPhase() const {
    return mode_ == DemoModeEnum::deep ? deep_sector : 1;
}

void DemoScenario::restart() {
    step_ = 0;
    log_timer_ = 0;
}

void DemoScenario::run(GameContextDto& context, float seconds) {
    log_timer_ -= context.clock.realDeltaSeconds();
    const bool should_log = log_timer_ <= 0;
    if (should_log) log_timer_ = log_interval_seconds;
    context.player.vitality().grantInvulnerability(1);
    switch (mode_) {
    case DemoModeEnum::none: break;
    case DemoModeEnum::play:
    case DemoModeEnum::deep:
        if (should_log && seconds > static_cast<float>(step_) * 5) {
            step_ += 1;
            logPatrols(context, seconds);
        }
        break;
    case DemoModeEnum::tracker: context.tracker.forceOpen(seconds > 1.5F); break;
    case DemoModeEnum::sectors: runSectors(context); break;
    case DemoModeEnum::showcase: runShowcase(context); break;
    case DemoModeEnum::freeze: runFreeze(context, seconds, should_log); break;
    case DemoModeEnum::moth: runMoth(context, should_log); break;
    case DemoModeEnum::locker: runLocker(context, seconds, should_log, false); break;
    case DemoModeEnum::witness: runLocker(context, seconds, should_log, true); break;
    case DemoModeEnum::repair: runRepair(context, seconds, should_log); break;
    case DemoModeEnum::power: runPower(context, should_log); break;
    case DemoModeEnum::grenade: runGrenade(context, seconds, should_log); break;
    case DemoModeEnum::lure: runLure(context, seconds, should_log); break;
    case DemoModeEnum::doors: runDoors(context, should_log); break;
    case DemoModeEnum::walk: runWalk(context, seconds, should_log); break;
    }
}

void DemoScenario::logPatrols(GameContextDto& context, float seconds) {
    const auto& mobs = context.mobs.all();
    for (std::size_t index = 0; index < mobs.size(); ++index) {
        const Mob& mob = *mobs[index];
        logLine(std::format("patrol t={:.0f} sector={} mob {} {} room={} intent={}", seconds, context.world.phase(), index, kindName(mob.kind()),
                            optionalIndex(context.world.map().roomIndexAt(mob.position())), nameOf(mob.intent())));
    }
}

void DemoScenario::spawnInSight(GameContextDto& context, MobKindEnum kind, float distance) {
    const TileMap& map = context.world.map();
    const Vec2 origin = context.player.position();
    for (int attempt = 0; attempt < 32; ++attempt) {
        const Vec2 spot = map.nearestFloor(origin.add(Vec2::fromAngle(static_cast<float>(attempt) * 0.39F).scale(distance)));
        if (!map.roomIndexAt(spot) || !map.hasLineOfSight(spot, origin)) continue;
        context.mobs.spawn(kind, spot);
        return;
    }
}

void DemoScenario::aimAtWorldOffset(GameContextDto& context, Vec2 offset) {
    Input& input = context.input;
    const float pixels_per_unit = input.windowSize().x / static_cast<float>(RenderConfig::view_width);
    input.simulateMousePosition(input.windowSize().scale(0.5F).add(offset.scale(pixels_per_unit)));
}

void DemoScenario::runSectors(GameContextDto& context) {
    World& world = context.world;
    Generators& generators = context.generators;
    if (world.phase() != last_phase_) {
        last_phase_ = world.phase();
        logLine(std::format("sectors: entered sector {} map={}x{} rooms={} generators={} fabricators={} mobs={}", world.phase(), world.map().width(), world.map().height(),
                            world.map().rooms().size(), generators.all().size(), generators.fabricators().size(), context.mobs.all().size()));
    }
    if (!generators.areAllOnline()) {
        for (Generator& generator : generators.all()) generator.forceOnline(context);
        return;
    }
    if (generators.exitDoor().openAmount() > 0.7F) context.player.setPosition(world.exitPosition().add(Vec2::of(0, 8)));
}

void DemoScenario::runShowcase(GameContextDto& context) {
    context.mobs.setGraceSeconds(1000);
    aimAtWorldOffset(context, Vec2::of(0, -100));
    if (step_ != 0) return;
    step_ = 1;
    context.mobs.clear();
    float offset = -66;
    for (const MobKindEnum kind : all_mob_kinds) {
        context.mobs.spawn(kind, context.world.map().nearestFloor(context.player.position().add(Vec2::of(offset, -52))));
        offset += 22;
    }
}

void DemoScenario::runFreeze(GameContextDto& context, float seconds, bool should_log) {
    const Player& player = context.player;
    context.mobs.setGraceSeconds(0);
    if (step_ == 0) {
        step_ = 1;
        context.mobs.clear();
        context.mobs.spawn(MobKindEnum::crawler, context.world.map().nearestFloor(player.position().add(Vec2::of(70, 0))));
        context.mobs.spawn(MobKindEnum::crawler, context.world.map().nearestFloor(player.position().add(Vec2::of(-70, 0))));
    }
    aimAtWorldOffset(context, Vec2::of(Scalar::floorModulo(seconds, 4) < 2 ? 160.0F : -160.0F, 0));
    if (!should_log) return;
    const auto& mobs = context.mobs.all();
    for (std::size_t index = 0; index < mobs.size(); ++index) {
        const Mob& mob = *mobs[index];
        logLine(std::format("freeze: mob {} side={} frozen={} intent={} distance={:.0f}", index, mob.position().x > player.position().x ? "right" : "left", mob.isFrozen(),
                            nameOf(mob.intent()), mob.distanceTo(player.position())));
    }
}

void DemoScenario::runMoth(GameContextDto& context, bool should_log) {
    Player& player = context.player;
    context.mobs.setGraceSeconds(0);
    player.flashlight().setEnabled(false);
    if (step_ == 0) {
        step_ = 1;
        context.mobs.clear();
        context.mobs.spawn(MobKindEnum::moth, player.position().add(Vec2::of(8, 0)));
        spawnInSight(context, MobKindEnum::crawler, 160);
    }
    if (!should_log) return;
    Vec2 moth_position{};
    std::string_view stance = "missing";
    for (const auto& mob : context.mobs.all()) {
        if (const auto* moth = dynamic_cast<const Moth*>(mob.get())) {
            moth_position = moth->position();
            stance = nameOf(moth->stance());
        }
    }
    for (const auto& mob : context.mobs.all()) {
        if (mob->kind() != MobKindEnum::crawler) continue;
        logLine(std::format("moth: stance={} crawler frozen={} intent={} distance_to_moth={:.0f} distance_to_player={:.0f}", stance, mob->isFrozen(), nameOf(mob->intent()),
                            mob->distanceTo(moth_position), mob->distanceTo(player.position())));
    }
}

void DemoScenario::runLocker(GameContextDto& context, float seconds, bool should_log, bool has_witness) {
    World& world = context.world;
    Player& player = context.player;
    context.mobs.setGraceSeconds(has_witness ? 0.0F : 1000.0F);
    if (has_witness) aimAtWorldOffset(context, Vec2::of(0, -200));
    if (step_ == 0 && !world.lockers().empty()) {
        step_ = 1;
        std::size_t nearest = 0;
        const auto& lockers = world.lockers();
        for (std::size_t index = 0; index < lockers.size(); ++index) {
            if (lockers[index].position().distance(player.position()) < lockers[nearest].position().distance(player.position())) nearest = index;
        }
        player.setPosition(world.map().nearestFloor(lockers[nearest].entryPoint()));
        context.camera.snapToPlayer(world, player);
        context.mobs.clear();
        if (has_witness) spawnInSight(context, MobKindEnum::crawler, 130);
    }
    if (step_ == 1 && seconds > 1.5F) {
        step_ = 2;
        context.input.simulatePress(InputActionEnum::interact);
    }
    if (step_ == 2 && seconds > 4.5F && !has_witness) {
        step_ = 3;
        context.input.simulatePress(InputActionEnum::interact);
    }
    if (!should_log) return;
    logLine(std::format("locker: step={} hidden={} locker={}", step_, player.isHidden(), optionalIndex(player.hiding().hiddenLockerIndex())));
    const auto& mobs = context.mobs.all();
    for (std::size_t index = 0; index < mobs.size(); ++index) {
        const Mob& mob = *mobs[index];
        logLine(std::format("locker: mob {} intent={} known_locker={} distance={:.0f}", index, nameOf(mob.intent()), optionalIndex(mob.memory().known_locker_index),
                            mob.distanceTo(player.position())));
    }
}

void DemoScenario::runRepair(GameContextDto& context, float seconds, bool should_log) {
    context.mobs.setGraceSeconds(1000);
    if (context.generators.all().empty()) return;
    Generator& generator = context.generators.all().front();
    if (step_ == 0) {
        step_ = 1;
        context.player.setPosition(context.world.map().nearestFloor(generator.position().add(Vec2::of(0, 30))));
        context.camera.snapToPlayer(context.world, context.player);
        generator.cancelPlannedIncidents();
    }
    context.input.simulateHold(InputActionEnum::interact, seconds < 40);
    const SkillCheck& check = generator.skillCheck();
    if (check.isActive() && !check.isWarning() && check.needleAngle() >= check.perfectStart() + check.perfectWidth() * 0.5F && seconds < 12) {
        context.input.simulatePress(InputActionEnum::hold_breath);
    }
    if (should_log) {
        logLine(std::format("repair: state={} progress={:.3f} check_active={} last={}", stateName(generator.state()), generator.progress(), check.isActive(), resultName(check.lastResult())));
    }
}

void DemoScenario::runPower(GameContextDto& context, bool should_log) {
    context.mobs.setGraceSeconds(1000);
    if (context.generators.all().empty()) return;
    Generator& generator = context.generators.all().front();
    if (step_ == 0) {
        step_ = 1;
        context.player.setPosition(context.world.map().nearestFloor(generator.position().add(Vec2::of(0, 34))));
        context.camera.snapToPlayer(context.world, context.player);
        generator.cancelPlannedIncidents();
        generator.setProgress(0.93F);
        generator.skillCheck().postpone(100);
    }
    context.input.simulateHold(InputActionEnum::interact, !generator.isOnline());
    if (!should_log) return;
    logLine(std::format("power: state={} lit={:.2f} burning_here={} charging={}", stateName(generator.state()), context.world.power().litAmount(generator.roomIndex()),
                        context.world.isBurningLightAt(context.player.position()), context.player.flashlight().battery().isCharging()));
}

void DemoScenario::runGrenade(GameContextDto& context, float seconds, bool should_log) {
    context.mobs.setGraceSeconds(1000);
    context.player.flashlight().setEnabled(false);
    aimAtWorldOffset(context, Vec2::of(90, 0));
    if (step_ == 0) {
        step_ = 1;
        context.mobs.clear();
    }
    if (step_ == 1 && seconds > 1.2F) {
        step_ = 2;
        const Vec2 target = context.camera.mouseWorldPosition(context.input);
        for (int index = 0; index < 3; ++index) {
            const auto order = static_cast<float>(index);
            context.mobs.spawn(MobKindEnum::crawler, context.world.map().nearestFloor(target.add(Vec2::of(-10 + order * 10, -8 + order * 8))));
        }
        context.input.simulatePress(InputActionEnum::throw_grenade);
    }
    if (!should_log) return;
    const auto neon = context.world.map().neonSeconds();
    const auto neon_cells = std::count_if(neon.begin(), neon.end(), [](float value) { return value > 0; });
    logLine(std::format("grenade: mobs_alive={} neon_cells={} puddles={}", context.mobs.all().size(), neon_cells, context.throwables.puddleCount()));
}

void DemoScenario::runLure(GameContextDto& context, float seconds, bool should_log) {
    Player& player = context.player;
    context.mobs.setGraceSeconds(0);
    player.flashlight().setEnabled(false);
    aimAtWorldOffset(context, Vec2::of(120, 0));
    if (step_ == 0) {
        step_ = 1;
        context.mobs.clear();
        context.mobs.spawn(MobKindEnum::crawler, context.world.map().nearestFloor(player.position().add(Vec2::of(-330, 0))));
    }
    if (step_ == 1 && seconds > 1) {
        step_ = 2;
        context.input.simulatePress(InputActionEnum::throw_lure);
    }
    if (!should_log || context.throwables.beacons().empty()) return;
    const Vec2 beacon = context.throwables.beacons().front().position();
    const auto& mobs = context.mobs.all();
    for (std::size_t index = 0; index < mobs.size(); ++index) {
        const Mob& mob = *mobs[index];
        logLine(std::format("lure: t={:.1f} mob {} intent={} distance_to_beacon={:.0f} distance_to_player={:.0f}", seconds, index, nameOf(mob.intent()), mob.distanceTo(beacon),
                            mob.distanceTo(player.position())));
    }
}

void DemoScenario::runDoors(GameContextDto& context, bool should_log) {
    World& world = context.world;
    context.mobs.setGraceSeconds(1000);
    const std::size_t start_room = world.startRoomIndex();
    if (step_ == 0) {
        step_ = 1;
        for (const DoorwayProps& doorway : world.map().doorways()) {
            if (!doorway.connects(start_room) || doorway.is_open) continue;
            const Vec2 room_center = world.map().rooms()[start_room].center();
            context.player.setPosition(world.map().nearestFloor(doorway.center().add(room_center.sub(doorway.center()).normalized().scale(60))));
            context.camera.snapToPlayer(world, context.player);
            break;
        }
    }
    world.mazeShifter().setTimerSeconds(std::min(world.mazeShifter().timerSeconds(), 1.5F));
    for (DoorwayProps& doorway : world.map().doorways()) doorway.seconds_since_change = std::max(doorway.seconds_since_change, 11.0F);
    for (const auto& moved : world.mazeShifter().movedDoorwayIndices()) {
        if (moved) total_moved_doorways_ += 1;
    }
    if (!should_log) return;
    logLine(std::format("doors: moved_total={} connected={} floor_connected={}", total_moved_doorways_, world.map().isRoomGraphConnected(), world.map().isEveryFloorConnected()));
}

void DemoScenario::runWalk(GameContextDto& context, float seconds, bool should_log) {
    World& world = context.world;
    Player& player = context.player;
    Input& input = context.input;
    context.mobs.setGraceSeconds(1000);
    const std::size_t start_room = world.startRoomIndex();
    std::optional<Vec2> target;
    for (const DoorwayProps& doorway : world.map().doorways()) {
        if (!doorway.connects(start_room) || !doorway.is_open) continue;
        const Vec2 other_center = world.map().rooms()[doorway.otherRoom(start_room)].center();
        target = doorway.center().add(other_center.sub(doorway.center()).normalized().scale(90));
        break;
    }
    for (const InputActionEnum action : {InputActionEnum::move_left, InputActionEnum::move_right, InputActionEnum::move_up, InputActionEnum::move_down}) input.simulateHold(action, false);
    if (!target) return;
    const bool reached = player.distanceTo(*target) < 12;
    if (!reached && step_ == 0) {
        const Vec2 direction = world.navigation().directionToward(world.map(), player.position(), *target, world.elapsedSeconds());
        const Vec2 steer = direction.length() < 0.5F ? target->sub(player.position()).normalized() : direction;
        input.simulateHold(InputActionEnum::move_left, steer.x < -0.38F);
        input.simulateHold(InputActionEnum::move_right, steer.x > 0.38F);
        input.simulateHold(InputActionEnum::move_up, steer.y < -0.38F);
        input.simulateHold(InputActionEnum::move_down, steer.y > 0.38F);
    } else if (step_ == 0) {
        step_ = 1;
    }
    aimAtWorldOffset(context, step_ == 1 && Scalar::floorModulo(seconds, 2) < 1 ? Vec2::of(200, 0) : Vec2::of(0, 0));
    if (!should_log) return;
    const Camera& camera = context.camera;
    logLine(std::format("walk: player_room={} camera_room={} transition={:.2f} camera_offset=({:.0f},{:.0f}) reached={}", optionalIndex(world.map().roomIndexAt(player.position())),
                        optionalIndex(camera.roomIndex()), camera.transitionSeconds(), camera.focus().x - player.position().x, camera.focus().y - player.position().y, step_ == 1));
}

}
