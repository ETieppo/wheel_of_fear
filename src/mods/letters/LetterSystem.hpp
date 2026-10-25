#pragma once

#include "core/text/enums/LanguageEnum.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/letters/LetterEntity.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace wheel_of_fear {
  class GeneratorSystem;

  class LetterSystem {
    public:
      LetterSystem();

      void generatePhase(
        WorldSystem           &world,
        const GeneratorSystem &generators,
        std::uint64_t          phase_seed
      );

      void clear();
      void update(GameContextDto &context);
      void draw(GameContextDto &context) const;

      [[nodiscard]]
      bool consumeReadingRequest();

      void finishReading(GameContextDto &context);

      void grantKey() {
        has_key_ = true;
      }

      [[nodiscard]]
      bool hasKey() const {
        return has_key_;
      }

      [[nodiscard]]
      const std::optional<LetterEntity> &letter() const {
        return letter_;
      }

      [[nodiscard]]
      std::string_view textFor(LanguageEnum language) const;

    private:
      static constexpr std::uint64_t placement_seed_salt = 0x1E77E21E77E2ULL;
      static constexpr float         locker_clearance    = 40;

      [[nodiscard]]
      static std::vector<std::size_t> candidateRooms(
        const WorldSystem     &world,
        const GeneratorSystem &generators
      );

      [[nodiscard]]
      static std::optional<Vec2Props> placementIn(
        const WorldSystem &world,
        std::size_t        room_index
      );

      void grantReward(GameContextDto &context) const;

      std::vector<std::string>    english_letters_;
      std::vector<std::string>    portuguese_letters_;
      std::optional<LetterEntity> letter_;
      std::uint32_t               phase_             = 1;
      bool                        has_key_           = false;
      bool                        is_read_requested_ = false;
  };
} // namespace wheel_of_fear
