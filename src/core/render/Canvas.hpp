#pragma once

#include <array>
#include <cstdint>
#include <span>

namespace wheel_of_fear {
  class Canvas {
    public:
      Canvas(
        std::span<std::uint8_t> pixels,
        std::uint32_t           size
      )
          : pixels_(pixels)
          , size_(size) {
      }

      void set(
        int x,
        int y,
        std::array<
          std::uint8_t,
          4
        > rgba
      );
      [[nodiscard]]
      std::array<
        std::uint8_t,
        4
      >
      get(
        int x,
        int y
      ) const;

      [[nodiscard]]
      std::span<std::uint8_t> pixels() const {
        return pixels_;
      }

      [[nodiscard]]
      std::uint32_t size() const {
        return size_;
      }

    private:
      [[nodiscard]]
      bool contains(
        int x,
        int y
      ) const;

      std::span<std::uint8_t> pixels_;
      std::uint32_t           size_;
  };
} // namespace wheel_of_fear
