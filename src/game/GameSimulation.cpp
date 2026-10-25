#include "game/GameSimulation.hpp"
#include "core/text/Localization.hpp"
#include <utility>

namespace wheel_of_fear {
  GameSimulation::GameSimulation(const UserDataFilesProps &user_data_files)
      : art_(renderer_)
      , director_(user_data_files)
      , context_ {
          .clock        = clock_,
          .input        = input_,
          .renderer     = renderer_,
          .sound_player = sound_player_,
          .art          = art_,
          .world        = world_,
          .lighting     = lighting_,
          .player       = player_,
          .mobs         = mobs_,
          .generators   = generators_,
          .effects      = effects_,
          .tracker      = tracker_,
          .hud          = hud_,
          .noise        = noise_,
          .throwables   = throwables_,
          .distractions = distractions_,
          .camera       = camera_,
          .extra_lives  = extra_lives_,
          .letters      = letters_,
        } {
    Director::initialize(context_);
  }

  void GameSimulation::step(float delta_seconds) {
    clock_.advance(delta_seconds);
    tick();
    draw();
  }

  GameSnapshotProps GameSimulation::snapshot() const {
    return GameSnapshotProps {
      .director = director_.snapshot(),
      .language = Localization::language(),
    };
  }

  void GameSimulation::resume(const GameSnapshotProps &snapshot) {
    Localization::setLanguage(snapshot.language);
    director_.resume(context_, snapshot.director);
  }

  void GameSimulation::tick() {
    lighting_.beginFrame();
    hud_.beginFrame(clock_.realDeltaSeconds());
    director_.update(context_);
    noise_.update(clock_.deltaSeconds());
    world_.update(context_);
    player_.update(context_);
    camera_.update(context_);
    tracker_.update(context_);
    throwables_.update(context_);
    extra_lives_.update(context_);
    letters_.update(context_);
    generators_.update(context_);
    distractions_.update(context_);
    mobs_.update(context_);
    effects_.update(context_);
    sound_player_.releaseFinished();
  }

  void GameSimulation::draw() {
    renderer_.beginFrame(camera_.viewCenter(), clock_.realSeconds());
    world_.draw(context_);
    generators_.draw(context_);
    throwables_.draw(context_);
    extra_lives_.draw(context_);
    letters_.draw(context_);
    mobs_.draw(context_);
    player_.draw(context_);
    effects_.draw(context_);
    lighting_.submitToRenderer(renderer_);
    hud_.draw(context_);
    tracker_.draw(context_);
    director_.draw(context_);
  }
} // namespace wheel_of_fear
