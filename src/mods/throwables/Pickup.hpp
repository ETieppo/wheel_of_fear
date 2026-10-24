#pragma once

#include <cstddef>

#include "core/items/enums/ItemEnum.hpp"
#include "core/math/Vec2.hpp"
#include "core/render/Renderer.hpp"
#include "mods/art/SpriteSheetProps.hpp"

namespace wheel_of_fear {
  class Pickup {
    public:
      static constexpr float reach = 13;

      Pickup(
        Vec2     position,
        ItemEnum item
      )
          : position_(position)
          , item_(item) {
      }

      Vec2 position() const {
        return position_;
      }

      ItemEnum item() const {
        return item_;
      }

      void settleOn(
        Vec2 floor
      ) {
        position_ = floor;
      }

      void draw(
        Renderer               &renderer,
        const SpriteSheetProps &sprites,
        std::size_t             index,
        float                   real_seconds
      ) const;

    private:
      Vec2     position_;
      ItemEnum item_;
  };
} // namespace wheel_of_fear
