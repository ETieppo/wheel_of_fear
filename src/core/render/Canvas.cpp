#include "core/render/Canvas.hpp"

#include <cstddef>
#include <utility>

namespace wheel_of_fear {
  bool Canvas::contains(
    int x,
    int y
  ) const {
    return x >=
           0 &&
           y >=
           0 &&
           std::cmp_less(x, size_) &&
           std::cmp_less(y, size_);
  }

  void Canvas::set(
    int x,
    int y,
    std::array<
      std::uint8_t,
      4
    > rgba
  ) {
    if(!contains(x, y)) return;

    const std::size_t offset =
      ((static_cast<std::size_t>(y) * size_) + static_cast<std::size_t>(x)) * 4;

    for(std::size_t channel = 0; channel < 4; ++channel)
      pixels_[offset + channel] = rgba[channel];
  }

  std::array<
    std::uint8_t,
    4>
  Canvas::get(
    int x,
    int y
  ) const {
    if(!contains(x, y)) return {0, 0, 0, 0};

    const std::size_t offset =
      ((static_cast<std::size_t>(y) * size_) + static_cast<std::size_t>(x)) * 4;

    return {
      pixels_[offset],
      pixels_[offset + 1],
      pixels_[offset + 2],
      pixels_[offset + 3],
    };
  }
} // namespace wheel_of_fear
