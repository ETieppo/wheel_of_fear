#pragma once

#include <string>
#include <vector>

#include "core/render/Renderer.hpp"
#include "mods/hud/HudMessageProps.hpp"

namespace wheel_of_fear {
  class MessageFeed {
    public:
      void clear() {
        messages_.clear();
      }

      void announce(
        Color       color,
        std::string text
      );
      void age(float delta_seconds);
      void draw(Renderer &renderer) const;

    private:
      static constexpr std::size_t capacity         = 5;
      static constexpr float       lifetime_seconds = 6;
      static constexpr float       fade_seconds     = 1.2F;

      std::vector<HudMessageProps> messages_;
  };
} // namespace wheel_of_fear
