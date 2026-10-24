#pragma once

#include <cstddef>
#include <optional>

#include "core/math/ColorProps.hpp"
#include "core/math/RandomGenerator.hpp"
#include "core/math/Vec2Props.hpp"
#include "core/render/AtlasRegionProps.hpp"
#include "core/render/Renderer.hpp"
#include "mods/art/SpriteSheetProps.hpp"
#include "mods/distractions/enums/DistractionKindEnum.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/world/dtos/SceneryPlacementDto.hpp"
#include "mods/world/dtos/SceneryViewDto.hpp"

namespace wheel_of_fear {
  class Scenery {
    public:
      explicit Scenery(
        const SceneryPlacementDto &placement
      );
      virtual ~Scenery() = default;

      [[nodiscard]]
      Vec2Props position() const {
        return position_;
      }

      [[nodiscard]]
      std::size_t roomIndex() const {
        return room_index_;
      }

      [[nodiscard]]
      bool isBroken() const {
        return is_broken_;
      }

      void update(
        float delta_seconds
      );
      void bump(
        RandomGenerator &random
      );

      [[nodiscard]]
      virtual std::optional<DistractionKindEnum> bumpDistraction() const = 0;
      virtual void                               draw(
        Renderer               &renderer,
        const SpriteSheetProps &sprites,
        const SceneryViewDto   &view
      ) const;

      virtual void emitLight(
        [[maybe_unused]] LightingSystem       &lighting,
        [[maybe_unused]] const SceneryViewDto &view
      ) const {
      }

    protected:
      [[nodiscard]]
      virtual const AtlasRegionProps &sprite(
        const SpriteSheetProps &sprites
      ) const = 0;

      [[nodiscard]]
      virtual const AtlasRegionProps* glowSprite(
        [[maybe_unused]] const SpriteSheetProps &sprites
      ) const {
        return nullptr;
      }

      virtual void reactToBump(
        [[maybe_unused]] RandomGenerator &random
      ) {
      }

      [[nodiscard]]
      ColorProps color() const {
        return color_;
      }

      [[nodiscard]]
      bool isMirrored() const {
        return mirrored_;
      }

      [[nodiscard]]
      float flickerSeed() const {
        return flicker_seed_;
      }

      void breakApart() {
        is_broken_ = true;
      }

      void tilt(
        float amount
      ) {
        tilt_ = amount;
      }

    private:
      static constexpr float wobble_duration_seconds = 0.8F;

      Vec2Props   position_;
      bool        mirrored_;
      std::size_t room_index_;
      ColorProps  color_;
      float       flicker_seed_;
      bool        is_broken_      = false;
      float       wobble_seconds_ = 0;
      float       tilt_           = 0;
  };
} // namespace wheel_of_fear
