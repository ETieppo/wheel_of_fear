#include "mods/letters/LetterSystem.hpp"
#include "core/audio/SoundPlayer.hpp"
#include "core/input/InputController.hpp"
#include "core/items/ItemCatalog.hpp"
#include "core/math/RandomGenerator.hpp"
#include "core/text/Localization.hpp"
#include "core/theme/Palette.hpp"
#include "core/time/GameClock.hpp"
#include "mods/art/ArtLibrary.hpp"
#include "mods/generators/GeneratorSystem.hpp"
#include "mods/hud/HudSystem.hpp"
#include "mods/letters/LetterCatalog.hpp"
#include "mods/lighting/LightingSystem.hpp"
#include "mods/player/PlayerEntity.hpp"
#include "mods/world/WorldSystem.hpp"
#include <algorithm>

namespace wheel_of_fear {
  namespace {
    constexpr LocalizedTextProps read_prompt {
      "read the letter",
      "ler a carta",
    };
    constexpr LocalizedTextProps key_found {
      "THE KEY TO THE EXIT WAS INSIDE THE LETTER",
      "A CHAVE DA SAÍDA ESTAVA DENTRO DA CARTA",
    };
    constexpr LocalizedTextProps reward_received {
      "RECEIVED  {}  x{}",
      "RECEBEU  {}  x{}",
    };
    constexpr LocalizedTextProps reward_lost {
      "YOUR BAG IS FULL  -  THE GIFT STAYS BEHIND",
      "SUA BOLSA ESTÁ CHEIA  -  O PRESENTE FICOU PRA TRÁS",
    };
  } // namespace

  LetterSystem::LetterSystem()
      : english_letters_(LetterCatalog::load(LanguageEnum::english))
      , portuguese_letters_(
          LetterCatalog::load(LanguageEnum::brazilian_portuguese)
        ) {
  }

  void LetterSystem::generatePhase(
    WorldSystem           &world,
    const GeneratorSystem &generators,
    std::uint64_t          phase_seed
  ) {
    clear();
    phase_ = world.phase();

    std::vector<std::size_t> rooms = candidateRooms(world, generators);
    RandomGenerator          random(phase_seed ^ placement_seed_salt);

    while(!rooms.empty()) {
      const std::size_t pick       = random.index(rooms.size());
      const std::size_t room_index = rooms[pick];

      if(const std::optional<Vec2Props> spot = placementIn(world, room_index)) {
        letter_.emplace(*spot);
        return;
      }

      rooms.erase(rooms.begin() + static_cast<std::ptrdiff_t>(pick));
    }

    letter_.emplace(world.map().nearestFloor(
      world.map().rooms()[world.startRoomIndex()].center()
    ));
  }

  void LetterSystem::clear() {
    letter_.reset();
    has_key_           = false;
    is_read_requested_ = false;
  }

  std::vector<std::size_t> LetterSystem::candidateRooms(
    const WorldSystem     &world,
    const GeneratorSystem &generators
  ) {
    const TileMapModel &map = world.map();
    std::vector<bool>   is_taken(map.rooms().size(), false);

    is_taken[world.startRoomIndex()] = true;
    is_taken[world.exitRoomIndex()]  = true;

    const auto take = [&](Vec2Props position) {
      if(const auto room = map.roomIndexAt(position)) is_taken[*room] = true;
    };

    for(const auto &generator : generators.all()) take(generator.position());
    for(const auto &fabricator : generators.fabricators())
      take(fabricator.position());

    std::vector<std::size_t> rooms;
    for(std::size_t room_index = 0; room_index < is_taken.size(); ++room_index)
      if(!is_taken[room_index]) rooms.push_back(room_index);

    return rooms;
  }

  std::optional<Vec2Props> LetterSystem::placementIn(
    const WorldSystem &world,
    std::size_t        room_index
  ) {
    const TileMapModel &map = world.map();
    const Vec2Props spot = map.nearestFloor(map.rooms()[room_index].center());

    if(map.roomIndexAt(spot) != room_index) return std::nullopt;

    const bool is_near_locker = std::ranges::any_of(
      world.lockers(),
      [spot](const LockerEntity &locker) {
        return locker.position().distance(spot) < locker_clearance;
      }
    );

    if(is_near_locker) return std::nullopt;
    return spot;
  }

  void LetterSystem::update(GameContextDto &context) {
    if(!letter_) return;

    letter_->emitSpotlight(context.lighting, context.clock.realSeconds());
    if(!context.clock.isRunning()) return;

    const TileMapModel &map = context.world.map();
    if(map.isSolidAt(letter_->position()))
      letter_->settleOn(map.nearestFloor(letter_->position()));

    PlayerEntity &player = context.player;
    if(!player.canBeTargeted()) return;
    if(letter_->position().distance(player.position()) >= LetterEntity::reach)
      return;

    context.hud.showPrompt(
      InputActionEnum::interact,
      std::string(Localization::text(read_prompt))
    );

    if(context.input.wasPressed(InputActionEnum::interact))
      is_read_requested_ = true;
  }

  bool LetterSystem::consumeReadingRequest() {
    const bool was_requested = is_read_requested_;
    is_read_requested_       = false;
    return was_requested && letter_.has_value();
  }

  std::string_view LetterSystem::textFor(LanguageEnum language) const {
    const std::vector<std::string> &letters
      = language == LanguageEnum::brazilian_portuguese ? portuguese_letters_
                                                       : english_letters_;
    if(letters.empty()) return {};

    const std::size_t index
      = std::min<std::size_t>(std::max(phase_, 1U) - 1, letters.size() - 1);
    return letters[index];
  }

  void LetterSystem::finishReading(GameContextDto &context) {
    if(!letter_) return;

    letter_.reset();
    has_key_ = true;
    context.player.vitality().grantInvulnerability(1);
    context.sound_player.play(SoundEnum::part_pickup, 0.8F, 0, 0.9F);
    context.hud.announce(
      Palette::accent,
      std::string(Localization::text(key_found))
    );
    grantReward(context);
  }

  void LetterSystem::grantReward(GameContextDto &context) const {
    const LetterRewardProps reward    = LetterCatalog::rewardFor(phase_);
    InventoryComponent     &inventory = context.player.inventory();
    int                     received  = 0;

    for(int index = 0; index < reward.count; ++index)
      if(inventory.receive(reward.item)) received += 1;

    if(received == 0) {
      context.hud.announce(
        Palette::muted_text,
        std::string(Localization::text(reward_lost))
      );
      return;
    }

    const Item &item = ItemCatalog::of(reward.item);
    context.hud.announce(
      item.colorForPhase(phase_),
      Localization::format(reward_received, item.displayName(), received)
    );
  }

  void LetterSystem::draw(GameContextDto &context) const {
    if(!letter_ || !context.renderer.isVisible(letter_->position(), 24)) return;

    letter_->draw(
      context.renderer,
      context.art.sprites(),
      context.clock.realSeconds()
    );
  }
} // namespace wheel_of_fear
