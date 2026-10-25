#pragma once

#include "game/dtos/GameContextDto.hpp"
#include "mods/extra_lives/ExtraLifePickupEntity.hpp"
#include <cstdint>
#include <optional>

namespace wheel_of_fear {
  class ExtraLifeSystem {
    public:
      static constexpr float spawn_chance = 0.2F;

      void generatePhase(
        WorldSystem  &world,
        std::uint64_t phase_seed,
        bool          can_grant_life
      );

      void update(GameContextDto &context);
      void draw(GameContextDto &context) const;

      [[nodiscard]]
      const std::optional<ExtraLifePickupEntity> &pickup() const {
        return pickup_;
      }

    private:
      static constexpr std::uint64_t spawn_seed_salt = 0x11FE11FE11FE11FEULL;

      void collect(GameContextDto &context);

      std::optional<ExtraLifePickupEntity> pickup_;
  };
} // namespace wheel_of_fear
