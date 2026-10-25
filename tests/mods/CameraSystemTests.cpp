#include "mods/camera/CameraSystem.hpp"
#include "support/HeadlessGameHarness.hpp"
#include <doctest/doctest.h>
#include <memory>

namespace wheel_of_fear {
  namespace {
    void checkCameraCentersOnPlayer(const GameContextDto &context) {
      const Vec2Props focus  = context.camera.focus();
      const Vec2Props player = context.player.position();

      CHECK(focus.x == doctest::Approx(player.x));
      CHECK(focus.y == doctest::Approx(player.y));
    }
  } // namespace

  TEST_CASE("the camera stays centered on the player regardless of the mouse") {
    const auto      harness = std::make_unique<HeadlessGameHarness>("play");
    GameContextDto &context = harness->context();
    const Vec2Props window  = context.input.windowSize();

    harness->advanceFrames(90);
    REQUIRE(context.clock.isRunning());

    context.input.simulateMousePosition(Vec2Props::of(window.x - 1, 1));
    harness->advanceFrames(5);
    checkCameraCentersOnPlayer(context);

    context.input.simulateMousePosition(Vec2Props::of(1, window.y - 1));
    context.input.simulateHold(InputActionEnum::move_right, true);
    harness->advanceFrames(40);
    checkCameraCentersOnPlayer(context);

    context.input.simulateHold(InputActionEnum::move_right, false);
    context.input.simulateHold(InputActionEnum::move_down, true);
    harness->advanceFrames(40);
    checkCameraCentersOnPlayer(context);

    context.input.simulateHold(InputActionEnum::move_down, false);
  }
} // namespace wheel_of_fear
