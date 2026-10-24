#include "support/HeadlessGameHarness.hpp"
#include <doctest/doctest.h>
#include <memory>

namespace wheel_of_fear {
  namespace {
    void holdForFrames(
      HeadlessGameHarness &harness,
      InputActionEnum      action,
      int                  frame_count
    ) {
      harness.context().input.simulateHold(action, true);
      harness.advanceFrames(frame_count);
      harness.context().input.simulateHold(action, false);
    }

    void checkAimsAtMouse(GameContextDto &context) {
      const Vec2Props toward_mouse
        = context.camera.mouseWorldPosition(context.input)
            .sub(context.player.position())
            .normalized();

      CHECK(context.player.aimDirection().dot(toward_mouse) > 0.99F);
    }
  } // namespace

  TEST_CASE("the flashlight follows the mouse, not the WASD direction") {
    const auto      harness = std::make_unique<HeadlessGameHarness>("play");
    GameContextDto &context = harness->context();
    const Vec2Props window  = context.input.windowSize();

    harness->advanceFrames(90);
    REQUIRE(context.clock.isRunning());

    context.input.simulateMousePosition(
      Vec2Props::of(window.x - 20, window.y / 2)
    );

    holdForFrames(*harness, InputActionEnum::move_left, 20);
    CHECK(context.player.aimDirection().x > 0.5F);
    checkAimsAtMouse(context);
    context.input.simulateMousePosition(Vec2Props::of(window.x / 2, 20));

    holdForFrames(*harness, InputActionEnum::move_down, 20);
    CHECK(context.player.aimDirection().y < -0.5F);
    checkAimsAtMouse(context);
  }
} // namespace wheel_of_fear
