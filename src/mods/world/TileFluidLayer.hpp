#pragma once

#include "core/math/Vec2Props.hpp"
#include <cstddef>
#include <span>
#include <vector>

namespace wheel_of_fear {
  class TileMapModel;

  class TileFluidLayer {
    public:
      explicit TileFluidLayer(const TileMapModel &map)
          : map_(map) {
      }

      [[nodiscard]]
      std::span<const float> acidSeconds() const {
        return acid_seconds_;
      }

      [[nodiscard]]
      std::span<const float> neonSeconds() const {
        return neon_seconds_;
      }

      void reset(std::size_t cell_count);
      void clearCell(std::size_t cell);
      [[nodiscard]]
      float acidAt(Vec2Props point) const;
      void  spillAcid(
        Vec2Props point,
        float     seconds
      );
      [[nodiscard]]
      float neonAt(Vec2Props point) const;
      [[nodiscard]]
      float neonAtTile(
        int x,
        int y
      ) const;
      void spillNeon(
        int   x,
        int   y,
        float seconds
      );
      void drainNeon(
        int x,
        int y
      );
      void evaporate(float seconds);

    private:
      const TileMapModel &map_;
      std::vector<float>  acid_seconds_;
      std::vector<float>  neon_seconds_;
  };
} // namespace wheel_of_fear
