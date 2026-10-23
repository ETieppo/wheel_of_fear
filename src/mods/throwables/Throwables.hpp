#pragma once

#include <memory>
#include <vector>

#include "core/items/enums/ItemEnum.hpp"
#include "core/math/Vec2.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/throwables/LureBeacon.hpp"
#include "mods/throwables/NeonPuddle.hpp"
#include "mods/throwables/Pickup.hpp"
#include "mods/throwables/projectiles/Projectile.hpp"

namespace wheel_of_fear {

class Throwables {
public:
    Throwables();
    ~Throwables();

    void generatePhase(World& world);
    void launch(ItemEnum item, Vec2 from, Vec2 toward);
    void deployBeacon(Vec2 landing) { beacons_.emplace_back(landing); }
    void spillPuddle(Vec2 landing) { puddles_.emplace_back(landing); }
    void update(GameContextDto& context);
    void draw(GameContextDto& context) const;

    const std::vector<LureBeacon>& beacons() const { return beacons_; }
    std::size_t puddleCount() const { return puddles_.size(); }

private:
    static constexpr float maximum_throw_distance = 190;

    void emitLights(GameContextDto& context) const;
    void advanceProjectiles(GameContextDto& context);
    void advanceBeacons(GameContextDto& context);
    void advancePuddles(float delta_seconds);
    void collectPickups(GameContextDto& context);

    std::vector<std::unique_ptr<Projectile>> projectiles_;
    std::vector<LureBeacon> beacons_;
    std::vector<NeonPuddle> puddles_;
    std::vector<Pickup> pickups_;
};

}
