#include "mods/director/states/TitleGameState.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/input/InputController.hpp"
#include "core/render/Renderer.hpp"
#include "core/time/GameClock.hpp"
#include "mods/camera/CameraSystem.hpp"
#include "mods/camera/RoomFramer.hpp"
#include "mods/director/Director.hpp"
#include "mods/director/screens/TitleScreen.hpp"
#include "mods/hud/HudSystem.hpp"
#include "mods/world/WorldSystem.hpp"
#include <cmath>

namespace wheel_of_fear {
  void TitleGameState::update(
    Director       &director,
    GameContextDto &context
  ) {
    context.clock.setRunning(false);
    context.hud.setVisible(false);
    panCamera(director, context);

    if(
      !context.input.wasPressed(InputActionEnum::confirm)
      && !director.demo().isActive()
    )
      return;

    context.sound_player.play(SoundEnum::ui_glitch, 0.8F, 0, 1);
    director.startPhase(context, director.demo().startingPhase());
  }

  void TitleGameState::mixMusic(GameContextDto &context) const {
    context.sound_player.setMusicMix(1, 0, 0);
  }

  void TitleGameState::draw(
    [[maybe_unused]] const Director &director,
    GameContextDto                  &context
  ) const {
    TitleScreen::draw(context.renderer, context.clock.realSeconds());
  }

  void TitleGameState::panCamera(
    const Director &director,
    GameContextDto &context
  ) {
    const auto &rooms = context.world.map().rooms();
    if(rooms.empty()) return;

    const float       state_seconds = director.stateSeconds();
    const std::size_t room_index
      = static_cast<std::size_t>(state_seconds / 9) % rooms.size();
    const RoomProps &room  = rooms[room_index];
    const Vec2Props  drift = Vec2Props::of(
      std::cos(state_seconds * 0.3F) * 50,
      std::sin(state_seconds * 0.4F) * 30
    );

    context.camera.glideTo(
      RoomFramer::clampToRoom(room, room.center().add(drift)),
      context.clock.realDeltaSeconds()
    );
  }
} // namespace wheel_of_fear
