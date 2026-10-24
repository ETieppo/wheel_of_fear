#pragma once

#include <cstdint>
#include <optional>

#include "core/items/enums/ItemEnum.hpp"
#include "core/math/ColorProps.hpp"
#include "core/math/Vec2Props.hpp"
#include "mods/tracker/enums/SignalKindEnum.hpp"

namespace wheel_of_fear {
  struct SignalProps {
      SignalKindEnum          kind = SignalKindEnum::unidentified;
      Vec2Props               position {};
      char                    label = '?';
      ColorProps              color = ColorProps::white();
      std::optional<ItemEnum> item;
      std::uint32_t           identity = 0;
  };
} // namespace wheel_of_fear
