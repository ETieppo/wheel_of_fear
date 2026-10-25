#include "mods/throwables/NeonPaintSpiller.hpp"
#include "core/math/RandomGenerator.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/RenderConfig.hpp"
#include "mods/world/WorldSystem.hpp"
#include <array>
#include <cmath>

namespace wheel_of_fear {
  namespace {
    struct CoreRingProps {
        float distance_fraction = 0;
        float size_fraction     = 0;
    };

    constexpr std::array<CoreRingProps, 3> core_rings {
      {
       {.distance_fraction = 0.0F, .size_fraction = 0.7F},
       {.distance_fraction = 0.45F, .size_fraction = 0.4F},
       {.distance_fraction = 0.78F, .size_fraction = 0.26F},
       },
    };
  } // namespace

  NeonSplashProps NeonPaintSpiller::spill(
    WorldSystem &world,
    Vec2Props    origin,
    float        reach,
    float        lifetime_seconds
  ) {
    RandomGenerator              &random = world.random();
    const NeonSplashOutlineProps  core   = shapeCore(random, origin, reach);
    std::vector<NeonDropletProps> droplets;

    addCore(random, core, droplets);
    addSpikes(random, core, reach, droplets);
    addSpray(random, origin, reach, droplets);
    floodTiles(world, origin, droplets, lifetime_seconds);

    NeonSplashProps splash {.lifetime_seconds = lifetime_seconds};

    for(const NeonDropletProps &candidate : droplets)
      if(world.map().fluids().neonAt(candidate.position) > 0)
        splash.droplets.push_back(candidate);

    return splash;
  }

  NeonSplashOutlineProps NeonPaintSpiller::shapeCore(
    RandomGenerator &random,
    Vec2Props        origin,
    float            reach
  ) {
    NeonSplashOutlineProps core {
      .center      = origin,
      .base_radius = reach * core_fraction,
    };

    const std::array<float, 3> phases {
      random.range(0, ScalarMath::tau),
      random.range(0, ScalarMath::tau),
      random.range(0, ScalarMath::tau),
    };

    for(
      std::size_t sample = 0; sample < NeonSplashOutlineProps::sample_count;
      ++sample
    ) {
      const float angle = ScalarMath::tau
        * static_cast<float>(sample)
        / static_cast<float>(NeonSplashOutlineProps::sample_count);

      core.radii[sample] = core.base_radius
        * (1
           + (0.14F * std::sin((3 * angle) + phases[0]))
           + (0.09F * std::sin((5 * angle) + phases[1]))
           + (0.06F * std::sin((7 * angle) + phases[2])));
    }

    return core;
  }

  void NeonPaintSpiller::addCore(
    RandomGenerator               &random,
    const NeonSplashOutlineProps  &core,
    std::vector<NeonDropletProps> &droplets
  ) {
    for(const CoreRingProps &ring : core_rings) {
      const std::size_t count = ring.distance_fraction <= 0
        ? 1
        : static_cast<std::size_t>(std::ceil(
            ScalarMath::tau * ring.distance_fraction / ring.size_fraction
          ))
          + 3;

      for(std::size_t index = 0; index < count; ++index) {
        const float angle = ScalarMath::tau
          * static_cast<float>(index)
          / static_cast<float>(count);
        const float edge = core.radiusAt(angle);

        droplets.push_back(droplet(
          random,
          core.center.add(
            Vec2Props::fromAngle(angle).scale(edge * ring.distance_fraction)
          ),
          edge * ring.size_fraction
        ));
      }
    }
  }

  void NeonPaintSpiller::addSpikes(
    RandomGenerator               &random,
    const NeonSplashOutlineProps  &core,
    float                          reach,
    std::vector<NeonDropletProps> &droplets
  ) {
    const std::size_t spike_count = spike_count_minimum
      + random.index(spike_count_maximum - spike_count_minimum + 1);

    for(std::size_t spike = 0; spike < spike_count; ++spike) {
      float angle = (ScalarMath::tau
                     * static_cast<float>(spike)
                     / static_cast<float>(spike_count))
        + random.range(-0.2F, 0.2F);
      const float bend = random.range(-spike_bend, spike_bend);
      const float root_radius
        = reach * spike_root_fraction * random.range(0.7F, 1.2F);
      const float start = core.radiusAt(angle) * 0.75F;
      const float length
        = reach * random.range(spike_minimum_reach, spike_maximum_reach);
      float travelled = 0;

      while(travelled < length) {
        const float progress = travelled / length;
        const float radius
          = root_radius * (1 - ((1 - spike_tip_taper) * progress));

        droplets.push_back(droplet(
          random,
          core.center.add(Vec2Props::fromAngle(angle).scale(start + travelled)),
          radius
        ));

        travelled += std::max(radius * 0.7F, 1.0F);
        angle += bend * 0.08F;
      }

      const float tip_gap    = root_radius * spike_tip_taper * 3;
      const float tip_radius = root_radius * spike_tip_taper * 1.4F;

      droplets.push_back(droplet(
        random,
        core.center.add(
          Vec2Props::fromAngle(angle).scale(start + length + tip_gap)
        ),
        tip_radius
      ));
    }
  }

  void NeonPaintSpiller::addSpray(
    RandomGenerator               &random,
    Vec2Props                      origin,
    float                          reach,
    std::vector<NeonDropletProps> &droplets
  ) {
    const std::size_t spray_count = spray_count_minimum
      + random.index(spray_count_maximum - spray_count_minimum + 1);

    for(std::size_t index = 0; index < spray_count; ++index) {
      const float distance
        = reach * random.range(spray_minimum_reach, spray_maximum_reach);

      droplets.push_back(droplet(
        random,
        origin.add(
          Vec2Props::fromAngle(random.range(0, ScalarMath::tau)).scale(distance)
        ),
        random.range(1, 3.2F)
      ));
    }
  }

  NeonDropletProps NeonPaintSpiller::droplet(
    RandomGenerator &random,
    Vec2Props        position,
    float            radius
  ) {
    return NeonDropletProps {
      .position = position,
      .radius   = radius,
      .seed     = random.unit(),
    };
  }

  bool NeonPaintSpiller::isTileUnderPaint(
    Vec2Props                            tile_center,
    const std::vector<NeonDropletProps> &droplets
  ) {
    const float margin = RenderConfig::tile_size * wet_reach_tile_margin;

    for(const NeonDropletProps &candidate : droplets)
      if(tile_center.distance(candidate.position) <= candidate.radius + margin)
        return true;

    return false;
  }

  void NeonPaintSpiller::floodTiles(
    WorldSystem                         &world,
    Vec2Props                            origin,
    const std::vector<NeonDropletProps> &droplets,
    float                                lifetime_seconds
  ) {
    constexpr std::array<TileCoordinateProps, 8> steps {
      {
       {.x = 1, .y = 0},
       {.x = -1, .y = 0},
       {.x = 0, .y = 1},
       {.x = 0, .y = -1},
       {.x = 1, .y = 1},
       {.x = 1, .y = -1},
       {.x = -1, .y = 1},
       {.x = -1, .y = -1},
       },
    };

    TileMapModel                    &map    = world.map();
    RandomGenerator                 &random = world.random();
    const TileCoordinateProps        start  = TileMapModel::tileAt(origin);
    std::vector<TileCoordinateProps> queue {start};

    map.fluids().spillNeon(start.x, start.y, lifetime_seconds);

    for(std::size_t head = 0; head < queue.size(); ++head) {
      const TileCoordinateProps current = queue[head];

      for(const TileCoordinateProps &step : steps) {
        const int x = current.x + step.x;
        const int y = current.y + step.y;
        if(map.isWall(x, y) || map.fluids().neonAtTile(x, y) > 0) continue;

        const bool is_diagonal = step.x != 0 && step.y != 0;
        const bool cuts_corner = is_diagonal
          && (map.isWall(current.x + step.x, current.y)
              || map.isWall(current.x, current.y + step.y));
        if(cuts_corner || queue.size() >= flood_capacity) continue;
        if(!isTileUnderPaint(TileMapModel::tileCenter(x, y), droplets))
          continue;

        map.fluids().spillNeon(x, y, lifetime_seconds - random.range(0, 6));
        queue.push_back({.x = x, .y = y});
      }
    }
  }
} // namespace wheel_of_fear
