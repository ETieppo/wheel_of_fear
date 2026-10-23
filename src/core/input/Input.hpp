#pragma once

#include <array>
#include <optional>

#include <SDL3/SDL_events.h>

#include "core/input/enums/InputActionEnum.hpp"
#include "core/math/Vec2.hpp"

namespace wheel_of_fear {
  class Input {
    public:
      void beginFrame();
      void handleEvent(const SDL_Event &event);

      bool isHeld(InputActionEnum action) const;
      bool wasPressed(InputActionEnum action) const;

      bool isQuitRequested() const {
        return is_quit_requested_;
      }

      Vec2 movementDirection() const;
      Vec2 mouseInView() const;
      Vec2 mouseInUi() const;

      Vec2 windowSize() const {
        return window_size_;
      }

      void setWindowSize(Vec2 window_size) {
        window_size_ = window_size;
      }

      void simulatePress(InputActionEnum action);
      void simulateHold(
        InputActionEnum action,
        bool            is_held
      );

      void simulateMousePosition(Vec2 mouse_position) {
        mouse_position_ = mouse_position;
      }

      static
      std::optional<InputActionEnum> actionForKey(SDL_Keycode key);

    private:
      void press(InputActionEnum action);
      void release(InputActionEnum action);
      Vec2 mouseInLetterbox(
        float width,
        float height
      ) const;

      std::array<bool, input_action_count> held_ {};
      std::array<bool, input_action_count> pressed_ {};
      Vec2                                 mouse_position_ {};
      Vec2                                 window_size_ = Vec2::of(1440, 810);
      bool                                 is_quit_requested_ = false;
  };
} // namespace wheel_of_fear
