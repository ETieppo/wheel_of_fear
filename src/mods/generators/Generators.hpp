#pragma once

#include <cstddef>
#include <optional>
#include <vector>

#include "game/dtos/GameContextDto.hpp"
#include "mods/generators/ExitDoor.hpp"
#include "mods/generators/Fabricator.hpp"
#include "mods/generators/Generator.hpp"

namespace wheel_of_fear {
  class Generators {
    public:
      void generatePhase(
        World &world
      );
      void update(
        GameContextDto &context
      );
      void draw(
        GameContextDto &context
      ) const;

      std::vector<Generator> &all() {
        return generators_;
      }

      const std::vector<Generator> &all() const {
        return generators_;
      }

      const std::vector<Fabricator> &fabricators() const {
        return fabricators_;
      }

      ExitDoor &exitDoor() {
        return exit_door_;
      }

      const ExitDoor &exitDoor() const {
        return exit_door_;
      }

      bool areAllOnline() const;

      bool hasPlayerExited() const {
        return exit_door_.hasPlayerExited();
      }

      const Generator* activeRepair() const;

    private:
      static constexpr float first_overload_seconds    = 75;
      static constexpr float overload_cooldown_seconds = 50;

      void placeGenerators(
        World             &world,
        std::vector<bool> &occupied_rooms
      );
      void placeFabricators(
        World             &world,
        std::vector<bool> &occupied_rooms
      );
      std::optional<std::size_t> pickDistantRoom(
        World                   &world,
        const std::vector<bool> &occupied_rooms
      ) const;
      void rollForOverload(
        GameContextDto &context
      );

      std::vector<Generator>  generators_;
      std::vector<Fabricator> fabricators_;
      ExitDoor                exit_door_;
      float                   overload_cooldown_ = 0;
  };
} // namespace wheel_of_fear
