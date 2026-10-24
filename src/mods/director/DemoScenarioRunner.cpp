#include "mods/director/DemoScenarioRunner.hpp"
#include "core/time/GameClock.hpp"
#include "mods/director/DemoModeParser.hpp"
#include "mods/director/demos/DemoScenarioFactory.hpp"
#include "mods/player/PlayerEntity.hpp"

namespace wheel_of_fear {
  DemoScenarioRunner::DemoScenarioRunner()
      : scenario_(
          DemoScenarioFactory::create(DemoModeParser::fromEnvironment())
        ) {
  }

  void DemoScenarioRunner::restart() {
    if(scenario_) scenario_->restart();
    log_timer_ = 0;
  }

  void DemoScenarioRunner::run(
    GameContextDto &context,
    float           seconds
  ) {
    log_timer_ -= context.clock.realDeltaSeconds();

    const bool should_log = log_timer_ <= 0;
    if(should_log) log_timer_ = log_interval_seconds;

    context.player.vitality().grantInvulnerability(1);
    if(!scenario_) return;

    scenario_->run(
      context,
      DemoTickDto {.seconds = seconds, .should_log = should_log}
    );
  }
} // namespace wheel_of_fear
