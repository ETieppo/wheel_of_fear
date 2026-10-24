#pragma once

#include "core/items/enums/ItemEnum.hpp"
#include "mods/generators/states/GeneratorState.hpp"

namespace wheel_of_fear {
  class GeneratorJammedState final: public GeneratorState {
    public:
      [[nodiscard]]
      ColorProps statusColor(const GeneratorEntity &generator) const override;

      void enter(
        GeneratorEntity &generator,
        GameContextDto  &context
      );

      void update(
        GeneratorEntity          &generator,
        GameContextDto           &context,
        const GeneratorUpdateDto &update
      ) override;

      void drawOverlay(
        const GeneratorEntity &generator,
        GameContextDto        &context
      ) const override;

    private:
      static void emitIdleSparks(
        GeneratorEntity &generator,
        GameContextDto  &context
      );

      void offerRequiredItem(
        GeneratorEntity &generator,
        GameContextDto  &context
      ) const;

      ItemEnum required_item_ = ItemEnum::gear;
  };
} // namespace wheel_of_fear
