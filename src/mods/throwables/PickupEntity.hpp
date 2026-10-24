#pragma once

#include "core/items/enums/ItemEnum.hpp"
#include "core/math/Vec2Props.hpp"
#include "core/render/Renderer.hpp"
#include "mods/art/SpriteSheetProps.hpp"
#include <cstddef>

namespace wheel_of_fear {
  class PickupEntity {
    public:
      static constexpr float reach = 13;

      PickupEntity(
        Vec2Props position,
        ItemEnum  item
      )
          : position_(position)
          , item_(item) {
      }

      [[nodiscard]]
      Vec2Props position() const {
        return position_;
      }

      [[nodiscard]]
      ItemEnum item() const {
        return item_;
      }

      void settleOn(Vec2Props floor) {
        position_ = floor;
      }

      void draw(
        Renderer               &renderer,
        const SpriteSheetProps &sprites,
        std::size_t             index,
        float                   real_seconds
      ) const;

    private:
      Vec2Props position_;
      ItemEnum  item_;
  };
} // namespace wheel_of_fear
