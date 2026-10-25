#pragma once

#include "game/dtos/GameContextDto.hpp"
#include "mods/generators/ExitDoorEntity.hpp"
#include "mods/generators/FabricatorEntity.hpp"
#include "mods/generators/GeneratorEntity.hpp"
#include <cstddef>
#include <optional>
#include <vector>

namespace wheel_of_fear {
  class GeneratorSystem {
    public:
      void generatePhase(WorldSystem &world);
      void clear();

      void update(GameContextDto &context);

      void draw(GameContextDto &context) const;

      std::vector<GeneratorEntity> &all() {
        return generators_;
      }

      [[nodiscard]]
      const std::vector<GeneratorEntity> &all() const {
        return generators_;
      }

      [[nodiscard]]
      const std::vector<FabricatorEntity> &fabricators() const {
        return fabricators_;
      }

      ExitDoorEntity &exitDoor() {
        return exit_door_;
      }

      [[nodiscard]]
      const ExitDoorEntity &exitDoor() const {
        return exit_door_;
      }

      [[nodiscard]]
      bool areAllOnline() const;

      [[nodiscard]]
      bool hasPlayerExited() const {
        return is_active_ && exit_door_.hasPlayerExited();
      }

      [[nodiscard]]
      const GeneratorEntity* activeRepair() const;

    private:
      void placeGenerators(
        WorldSystem       &world,
        std::vector<bool> &occupied_rooms
      );

      void placeFabricators(
        WorldSystem       &world,
        std::vector<bool> &occupied_rooms
      );

      std::optional<std::size_t> pickDistantRoom(
        WorldSystem             &world,
        const std::vector<bool> &occupied_rooms
      ) const;

      std::vector<GeneratorEntity>  generators_;
      std::vector<FabricatorEntity> fabricators_;
      ExitDoorEntity                exit_door_;
      bool                          is_active_ = false;
  };
} // namespace wheel_of_fear
