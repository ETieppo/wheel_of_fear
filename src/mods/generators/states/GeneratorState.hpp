#pragma once

#include "core/math/ColorProps.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/generators/dtos/GeneratorUpdateDto.hpp"

namespace wheel_of_fear {
  class GeneratorEntity;

  class GeneratorState {
    public:
      GeneratorState() = default;
      GeneratorState(
        const GeneratorState &
      ) = default;
      GeneratorState &operator =(
        const GeneratorState &
      )                         = default;
      virtual ~GeneratorState() = default;

      [[nodiscard]]
      virtual ColorProps statusColor(
        const GeneratorEntity &generator
      ) const = 0;

      [[nodiscard]]
      virtual bool canBeRepaired() const {
        return false;
      }

      virtual void update(
        GeneratorEntity          &generator,
        GameContextDto           &context,
        const GeneratorUpdateDto &update
      ) = 0;

      virtual void drawOverlay(
        [[maybe_unused]] const GeneratorEntity &generator,
        [[maybe_unused]] GameContextDto        &context
      ) const {
      }
  };
} // namespace wheel_of_fear
