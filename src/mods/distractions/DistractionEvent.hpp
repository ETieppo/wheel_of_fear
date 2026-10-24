#pragma once

#include "core/math/Vec2Props.hpp"
#include "mods/distractions/DistractionTraitsProps.hpp"
#include "mods/distractions/enums/DistractionKindEnum.hpp"
#include "mods/lighting/LightingSystem.hpp"

namespace wheel_of_fear {
  class DistractionEvent {
    public:
      DistractionEvent(
        DistractionKindEnum kind,
        Vec2Props           position
      );

      [[nodiscard]]
      bool isFinished() const;

      void advance(
        float delta_seconds
      ) {
        elapsed_seconds_ += delta_seconds;
      }

      void emitFlash(
        LightingSystem &lighting
      ) const;

    private:
      [[nodiscard]]
      float flashStrength() const;

      DistractionTraitsProps traits_;
      Vec2Props              position_;
      float                  elapsed_seconds_ = 0;
  };
} // namespace wheel_of_fear
