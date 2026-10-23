#pragma once

#include <cstdint>
#include <optional>

#include "core/items/enums/ItemEnum.hpp"
#include "core/math/Color.hpp"
#include "core/math/Vec2.hpp"
#include "mods/tracker/enums/SignalKindEnum.hpp"

namespace wheel_of_fear {
  struct SignalProps {
      SignalKindEnum          kind = SignalKindEnum::unidentified;
      Vec2                    position {};
      char                    label = '?';
      Color                   color = Color::white();
      std::optional<ItemEnum> item;
      std::uint32_t           identity = 0;
  };
} // namespace wheel_of_fear
