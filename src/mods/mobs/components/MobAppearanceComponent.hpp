#pragma once

#include "core/math/Vec2Props.hpp"
#include "core/render/AtlasRegionProps.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/mobs/dtos/MobLookDto.hpp"

namespace wheel_of_fear {
  class MobEntity;

  class MobAppearanceComponent {
    public:
      explicit MobAppearanceComponent(MobEntity &owner)
          : owner_(owner) {
      }

      [[nodiscard]]
      float opacity() const {
        return opacity_;
      }

      void setOpacity(float opacity) {
        opacity_ = opacity;
      }

      [[nodiscard]]
      float visibility() const {
        return visibility_;
      }

      void setVisibility(float visibility) {
        visibility_ = visibility;
      }

      [[nodiscard]]
      float presence() const {
        return opacity_ * visibility_;
      }

      void refreshVisibility(const GameContextDto &context);

      void drawBody(
        GameContextDto         &context,
        const AtlasRegionProps &body,
        const AtlasRegionProps &glow,
        const MobLookDto       &look
      ) const;

    private:
      static constexpr float visibility_gain = 2.4F;

      [[nodiscard]]
      float glowLevel() const;

      MobEntity &owner_;
      float      opacity_    = 1;
      float      visibility_ = 0;
  };
} // namespace wheel_of_fear
