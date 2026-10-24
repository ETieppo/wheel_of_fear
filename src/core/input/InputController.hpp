#pragma once

#include "core/input/enums/InputActionEnum.hpp"
#include "core/math/Vec2Props.hpp"
#include <SDL3/SDL_events.h>
#include <array>
#include <optional>

namespace wheel_of_fear {
  class InputController {
    public:
      void beginFrame();
      void handleEvent(const SDL_Event &event);

      [[nodiscard]]
      bool isHeld(InputActionEnum action) const;

      [[nodiscard]]
      bool wasPressed(InputActionEnum action) const;

      [[nodiscard]]
      bool isQuitRequested() const {
        return is_quit_requested_;
      }

      [[nodiscard]]
      Vec2Props movementDirection() const;

      [[nodiscard]]
      Vec2Props mouseInView() const;

      [[nodiscard]]
      Vec2Props mouseInUi() const;

      [[nodiscard]]
      Vec2Props windowSize() const {
        return window_size_;
      }

      void setWindowSize(Vec2Props window_size) {
        window_size_ = window_size;
      }

      void simulateMousePosition(Vec2Props mouse_position) {
        mouse_position_ = mouse_position;
      }

      void simulatePress(InputActionEnum action);

      void simulateHold(
        InputActionEnum action,
        bool            is_held
      );

      static std::optional<InputActionEnum> actionForKey(SDL_Keycode key);

    private:
      [[nodiscard]]
      Vec2Props mouseInLetterbox(
        float width,
        float height
      ) const;

      void press(InputActionEnum action);
      void release(InputActionEnum action);

      std::array<bool, input_action_count> held_ {};
      std::array<bool, input_action_count> pressed_ {};
      Vec2Props                            mouse_position_ {};
      Vec2Props window_size_       = Vec2Props::of(1440, 810);
      bool      is_quit_requested_ = false;
  };
} // namespace wheel_of_fear
