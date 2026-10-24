#include "mods/tracker/TrackerSignalCollector.hpp"
#include "core/items/ItemCatalog.hpp"
#include "core/theme/Palette.hpp"
#include "mods/generators/GeneratorSystem.hpp"
#include "mods/mobs/MobSystem.hpp"
#include "mods/tracker/TrackerConfig.hpp"
#include "mods/world/WorldSystem.hpp"
#include <cstdint>

namespace wheel_of_fear {
  TrackerSignalCollector::TrackerSignalCollector() {
    signals_.reserve(TrackerConfig::signal_capacity);
  }

  void TrackerSignalCollector::collect(
    const GameContextDto              &context,
    const std::vector<GhostEchoProps> &ghosts
  ) {
    signals_.clear();
    collectGenerators(context);
    collectFabricators(context);
    collectExit(context);
    collectDisguisedMobs(context);
    collectGhosts(ghosts);
  }

  void TrackerSignalCollector::addSignal(const SignalProps &signal) {
    if(signals_.size() >= TrackerConfig::signal_capacity) return;
    signals_.push_back(signal);
  }

  void
  TrackerSignalCollector::collectGenerators(const GameContextDto &context) {
    const auto &generators = context.generators.all();

    for(std::size_t index = 0; index < generators.size(); ++index) {
      const GeneratorEntity &generator = generators[index];
      ColorProps             color     = Palette::tracker_green;

      if(
        generator.state()
        == GeneratorStateEnum::jammed
        || generator.state()
        == GeneratorStateEnum::broken
      )
        color = Palette::spark_yellow;
      if(generator.isOnline()) color = Palette::success;

      addSignal(
        SignalProps {
          .kind     = SignalKindEnum::generator,
          .position = generator.position(),
          .label    = generator.label(),
          .color    = color,
          .item     = std::nullopt,
          .identity = static_cast<std::uint32_t>(index),
        }
      );
    }
  }

  void
  TrackerSignalCollector::collectFabricators(const GameContextDto &context) {
    const auto &fabricators = context.generators.fabricators();

    for(std::size_t index = 0; index < fabricators.size(); ++index) {
      const FabricatorEntity &fabricator = fabricators[index];
      const ItemEnum          item       = fabricator.producedItem();

      addSignal(
        SignalProps {
          .kind     = SignalKindEnum::fabricator,
          .position = fabricator.position(),
          .label    = '?',
          .color    = ItemCatalog::of(item).color(),
          .item     = item,
          .identity = static_cast<std::uint32_t>(100 + index),
        }
      );
    }
  }

  void TrackerSignalCollector::collectExit(const GameContextDto &context) {
    const ColorProps exit_color = context.generators.exitDoor().isUnlocked()
      ? Palette::success
      : Palette::danger;

    addSignal(
      SignalProps {
        .kind     = SignalKindEnum::exit,
        .position = context.world.exitPosition(),
        .label    = '?',
        .color    = exit_color,
        .item     = std::nullopt,
        .identity = 200,
      }
    );
  }

  void
  TrackerSignalCollector::collectDisguisedMobs(const GameContextDto &context) {
    const auto &mobs = context.mobs.all();

    for(std::size_t index = 0; index < mobs.size(); ++index) {
      if(!mobs[index]->isDisguised()) continue;
      const auto disguise = static_cast<ItemEnum>(index % item_count);

      addSignal(
        SignalProps {
          .kind     = SignalKindEnum::fabricator,
          .position = mobs[index]->position(),
          .label    = '?',
          .color    = ItemCatalog::of(disguise).color(),
          .item     = disguise,
          .identity = static_cast<std::uint32_t>(300 + index),
        }
      );
    }
  }

  void TrackerSignalCollector::collectGhosts(
    const std::vector<GhostEchoProps> &ghosts
  ) {
    for(std::size_t index = 0; index < ghosts.size(); ++index) {
      addSignal(
        SignalProps {
          .kind     = SignalKindEnum::unidentified,
          .position = ghosts[index].position,
          .label    = '?',
          .color    = Palette::tracker_green.withAlpha(0.7F),
          .item     = std::nullopt,
          .identity = static_cast<std::uint32_t>(400 + index),
        }
      );
    }
  }
} // namespace wheel_of_fear
