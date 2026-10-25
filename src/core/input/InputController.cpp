#include "core/input/InputController.hpp"
#include "core/render/RenderConfig.hpp"
#include <algorithm>
#include <cstddef>

namespace wheel_of_fear {
  namespace {
    std::size_t slotOf(InputActionEnum action) {
      return static_cast<std::size_t>(action);
    }
  } // namespace

  void InputController::beginFrame() {
    pressed_.fill(false);
  }

  std::optional<InputActionEnum>
  InputController::actionForKey(SDL_Keycode key) {
    switch(key) {
      case SDLK_W:
      case SDLK_UP:     return InputActionEnum::move_up;
      case SDLK_S:
      case SDLK_DOWN:   return InputActionEnum::move_down;
      case SDLK_A:
      case SDLK_LEFT:   return InputActionEnum::move_left;
      case SDLK_D:
      case SDLK_RIGHT:  return InputActionEnum::move_right;
      case SDLK_LSHIFT:
      case SDLK_RSHIFT: return InputActionEnum::run;
      case SDLK_E:      return InputActionEnum::interact;
      case SDLK_SPACE:  return InputActionEnum::hold_breath;
      case SDLK_G:      return InputActionEnum::throw_grenade;
      case SDLK_TAB:
      case SDLK_Q:      return InputActionEnum::toggle_tracker;
      case SDLK_F:      return InputActionEnum::switch_light_mode;
      case SDLK_RETURN: return InputActionEnum::confirm;
      case SDLK_ESCAPE: return InputActionEnum::back;
      case SDLK_R:      return InputActionEnum::restart;
      case SDLK_M:      return InputActionEnum::toggle_mute;
      case SDLK_C:      return InputActionEnum::show_credits;
      case SDLK_X:      return InputActionEnum::quit_game;
      default:          return std::nullopt;
    }
  }

  void InputController::handleEvent(const SDL_Event &event) {
    switch(event.type) {
      case SDL_EVENT_QUIT:
      case SDL_EVENT_WINDOW_CLOSE_REQUESTED: is_quit_requested_ = true; break;
      case SDL_EVENT_KEY_DOWN:
        if(event.key.repeat) break;
        if(const auto action = actionForKey(event.key.key)) press(*action);
        break;
      case SDL_EVENT_KEY_UP:
        if(const auto action = actionForKey(event.key.key)) release(*action);
        break;
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if(event.button.button == SDL_BUTTON_LEFT) press(InputActionEnum::hug);
        if(event.button.button == SDL_BUTTON_RIGHT)
          press(InputActionEnum::throw_lure);
        break;
      case SDL_EVENT_MOUSE_BUTTON_UP:
        if(event.button.button == SDL_BUTTON_LEFT)
          release(InputActionEnum::hug);
        if(event.button.button == SDL_BUTTON_RIGHT)
          release(InputActionEnum::throw_lure);
        break;
      case SDL_EVENT_MOUSE_MOTION:
        mouse_position_ = Vec2Props::of(event.motion.x, event.motion.y);
        break;
      case SDL_EVENT_WINDOW_FOCUS_LOST: held_.fill(false); break;
      case SDL_EVENT_WINDOW_RESIZED:
        window_size_ = Vec2Props::of(
          static_cast<float>(event.window.data1),
          static_cast<float>(event.window.data2)
        );
        break;
      default: break;
    }
  }

  void InputController::press(InputActionEnum action) {
    held_[slotOf(action)]    = true;
    pressed_[slotOf(action)] = true;
  }

  void InputController::release(InputActionEnum action) {
    held_[slotOf(action)] = false;
  }

  void InputController::simulatePress(InputActionEnum action) {
    pressed_[slotOf(action)] = true;
  }

  void InputController::simulateHold(
    InputActionEnum action,
    bool            is_held
  ) {
    held_[slotOf(action)] = is_held;
  }

  bool InputController::isHeld(InputActionEnum action) const {
    return held_[slotOf(action)];
  }

  bool InputController::wasPressed(InputActionEnum action) const {
    return pressed_[slotOf(action)];
  }

  Vec2Props InputController::movementDirection() const {
    Vec2Props direction {};
    if(isHeld(InputActionEnum::move_left)) direction.x -= 1;
    if(isHeld(InputActionEnum::move_right)) direction.x += 1;
    if(isHeld(InputActionEnum::move_up)) direction.y -= 1;
    if(isHeld(InputActionEnum::move_down)) direction.y += 1;
    return direction.normalized();
  }

  Vec2Props InputController::mouseInLetterbox(
    float width,
    float height
  ) const {
    const float scale
      = std::min(window_size_.x / width, window_size_.y / height);
    const Vec2Props offset = Vec2Props::of(
      (window_size_.x - (width * scale)) / 2,
      (window_size_.y - (height * scale)) / 2
    );

    return mouse_position_.sub(offset).scale(1.0F / scale);
  }

  Vec2Props InputController::mouseInView() const {
    return mouseInLetterbox(
      static_cast<float>(RenderConfig::view_width),
      static_cast<float>(RenderConfig::view_height)
    );
  }

  Vec2Props InputController::mouseInUi() const {
    return mouseInLetterbox(RenderConfig::ui_width, RenderConfig::ui_height);
  }
} // namespace wheel_of_fear
