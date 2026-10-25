#include "mods/generators/states/GeneratorJammedState.hpp"
#include "core/input/InputController.hpp"
#include "core/items/ItemCatalog.hpp"
#include "core/math/ScalarMath.hpp"
#include "core/render/Renderer.hpp"
#include "core/text/Localization.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/effects/EffectSystem.hpp"
#include "mods/generators/GeneratorEntity.hpp"
#include "mods/hud/HudSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <cmath>
#include <format>
#include <string_view>

namespace wheel_of_fear {
  namespace {
    constexpr LocalizedTextProps jammed_message {
      "GENERATOR {} JAMMED  -  IT NEEDS A {}",
      "GERADOR {} TRAVADO  -  PRECISA DE {}",
    };
    constexpr LocalizedTextProps needs_part_prompt {
      "GENERATOR {} NEEDS A {}  -  FIND A FABRICATOR",
      "GERADOR {} PRECISA DE {}  -  ENCONTRE UM FABRICADOR",
    };
    constexpr LocalizedTextProps insert_part_prompt {
      "insert {} into generator {}",
      "inserir {} no gerador {}",
    };
    constexpr LocalizedTextProps unjammed_message {
      "GENERATOR {} UNJAMMED",
      "GERADOR {} DESTRAVADO",
    };
  } // namespace

  ColorProps GeneratorJammedState::statusColor(
    [[maybe_unused]] const GeneratorEntity &generator
  ) const {
    return Palette::spark_yellow;
  }

  void GeneratorJammedState::enter(
    GeneratorEntity &generator,
    GameContextDto  &context
  ) {
    const auto parts = ItemCatalog::generatorParts();
    required_item_   = parts[context.world.random().index(parts.size())];

    context.effects.emitSparks(
      context.world,
      generator.position(),
      10,
      Palette::spark_yellow
    );

    generator.playSound(context, SoundEnum::generator_request, 0.9F, 1);
    context.hud.announce(
      Palette::spark_yellow,
      Localization::format(
        jammed_message,
        generator.label(),
        ItemCatalog::of(required_item_).displayName()
      )
    );
  }

  void GeneratorJammedState::update(
    GeneratorEntity          &generator,
    GameContextDto           &context,
    const GeneratorUpdateDto &update
  ) {
    GeneratorMachineryProps &machinery = generator.machinery();
    machinery.rotor_speed              = ScalarMath::approach(
      machinery.rotor_speed,
      0,
      context.clock.deltaSeconds() * 6
    );

    emitIdleSparks(generator, context);
    if(!update.is_player_near) return;
    offerRequiredItem(generator, context);
  }

  void GeneratorJammedState::emitIdleSparks(
    GeneratorEntity &generator,
    GameContextDto  &context
  ) {
    GeneratorMachineryProps &machinery = generator.machinery();

    machinery.spark_timer -= context.clock.deltaSeconds();
    if(machinery.spark_timer > 0) return;

    machinery.spark_timer = context.world.random().range(0.6F, 1.6F);
    context.effects.emitSparks(
      context.world,
      generator.position().add(Vec2Props::of(-4, -2)),
      3,
      Palette::spark_yellow
    );
  }

  void GeneratorJammedState::offerRequiredItem(
    GeneratorEntity &generator,
    GameContextDto  &context
  ) const {
    const std::string_view item_name
      = ItemCatalog::of(required_item_).displayName();

    InventoryComponent &inventory = context.player.inventory();
    if(!inventory.has(required_item_)) {
      context.hud.showPrompt(
        std::nullopt,
        Localization::format(needs_part_prompt, generator.label(), item_name)
      );

      return;
    }

    context.hud.showPrompt(
      InputActionEnum::interact,
      Localization::format(insert_part_prompt, item_name, generator.label())
    );

    if(!context.input.wasPressed(InputActionEnum::interact)) return;
    inventory.consume(required_item_);
    generator.enterCharging();
    generator.playSound(context, SoundEnum::part_pickup, 0.7F, 0.8F);
    generator.playSound(context, SoundEnum::generator_tick, 0.8F, 0.6F);

    context.hud.announce(
      Palette::success,
      Localization::format(unjammed_message, generator.label())
    );
  }

  void GeneratorJammedState::drawOverlay(
    const GeneratorEntity &generator,
    GameContextDto        &context
  ) const {
    const float bob = std::sin(context.clock.realSeconds() * 4) * 2;

    context.renderer.drawGlowSprite(
      context.art.sprites().itemIcon(required_item_),
      generator.position().add(Vec2Props::of(0, -32 + bob)),
      ItemCatalog::of(required_item_).color(),
      SpriteDrawDto {.scale = 1.2F}
    );
  }
} // namespace wheel_of_fear
