#pragma once

#include <optional>
#include <string>

#include "core/input/enums/InputActionEnum.hpp"
#include "core/render/Renderer.hpp"

namespace wheel_of_fear {
  class PromptBarWidget {
    public:
      void clear();
      void show(
        std::optional<InputActionEnum> key,
        std::string                    text
      );
      void draw(
        Renderer &renderer
      ) const;

    private:
      std::string                    text_;
      std::optional<InputActionEnum> key_;
  };
} // namespace wheel_of_fear
