#pragma once

#include "core/math/Vec2Props.hpp"
#include "mods/throwables/NeonSplashOutlineProps.hpp"
#include "mods/world/NeonSplashProps.hpp"
#include <cstddef>
#include <vector>

namespace wheel_of_fear {
  class WorldSystem;
  class RandomGenerator;

  class NeonPaintSpiller final {
    public:
      NeonPaintSpiller() = delete;

      static NeonSplashProps spill(
        WorldSystem &world,
        Vec2Props    origin,
        float        reach,
        float        lifetime_seconds
      );

    private:
      static constexpr float       core_fraction         = 0.38F;
      static constexpr std::size_t spike_count_minimum   = 9;
      static constexpr std::size_t spike_count_maximum   = 14;
      static constexpr float       spike_minimum_reach   = 0.5F;
      static constexpr float       spike_maximum_reach   = 1.0F;
      static constexpr float       spike_root_fraction   = 0.16F;
      static constexpr float       spike_tip_taper       = 0.22F;
      static constexpr float       spike_bend            = 0.12F;
      static constexpr std::size_t spray_count_minimum   = 14;
      static constexpr std::size_t spray_count_maximum   = 22;
      static constexpr float       spray_minimum_reach   = 0.55F;
      static constexpr float       spray_maximum_reach   = 1.3F;
      static constexpr std::size_t flood_capacity        = 220;
      static constexpr float       wet_reach_tile_margin = 0.35F;

      static NeonSplashOutlineProps shapeCore(
        RandomGenerator &random,
        Vec2Props        origin,
        float            reach
      );

      static void addCore(
        RandomGenerator               &random,
        const NeonSplashOutlineProps  &core,
        std::vector<NeonDropletProps> &droplets
      );

      static void addSpikes(
        RandomGenerator               &random,
        const NeonSplashOutlineProps  &core,
        float                          reach,
        std::vector<NeonDropletProps> &droplets
      );

      static void addSpray(
        RandomGenerator               &random,
        Vec2Props                      origin,
        float                          reach,
        std::vector<NeonDropletProps> &droplets
      );

      static NeonDropletProps droplet(
        RandomGenerator &random,
        Vec2Props        position,
        float            radius
      );

      [[nodiscard]]
      static bool isTileUnderPaint(
        Vec2Props                            tile_center,
        const std::vector<NeonDropletProps> &droplets
      );

      static void floodTiles(
        WorldSystem                         &world,
        Vec2Props                            origin,
        const std::vector<NeonDropletProps> &droplets,
        float                                lifetime_seconds
      );
  };
} // namespace wheel_of_fear
