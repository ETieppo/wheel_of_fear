#pragma once

#include "core/math/Vec2.hpp"
#include "mods/distractions/DistractionTraitsProps.hpp"
#include "mods/distractions/enums/DistractionKindEnum.hpp"
#include "mods/lighting/Lighting.hpp"

namespace wheel_of_fear {
  class Distraction {
    public:
      Distraction(
        DistractionKindEnum kind,
        Vec2                position
      );

      bool isFinished() const;

      void advance(float delta_seconds) {
        elapsed_seconds_ += delta_seconds;
      }

      void emitFlash(Lighting &lighting) const;

    private:
      float flashStrength() const;

      DistractionTraitsProps traits_;
      Vec2                   position_;
      float                  elapsed_seconds_ = 0;
  };
} // namespace wheel_of_fear
