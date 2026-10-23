#pragma once

#include <cstddef>
#include <optional>

#include "core/entity/Entity.hpp"
#include "core/math/Vec2.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/flashlight/Flashlight.hpp"
#include "mods/player/HugController.hpp"
#include "mods/player/Inventory.hpp"
#include "mods/player/LockerHiding.hpp"
#include "mods/player/PlayerMovement.hpp"
#include "mods/player/ThrowingArm.hpp"
#include "mods/player/Vitality.hpp"
#include "mods/player/enums/PlayerActivityEnum.hpp"

namespace wheel_of_fear {
  class Player final: public Entity {
    public:
      static constexpr float collision_half_size = 4.5F;

      Player();

      void respawn(Vec2 position);
      void update(GameContextDto &context);
      void draw(GameContextDto &context) const;
      void beginRepair(Vec2 target);
      void takeHit(
        GameContextDto &context,
        Vec2            source
      );
      void dragOutOfLocker(GameContextDto &context);

      Flashlight &flashlight() {
        return flashlight_;
      }

      const Flashlight &flashlight() const {
        return flashlight_;
      }

      Inventory &inventory() {
        return inventory_;
      }

      const Inventory &inventory() const {
        return inventory_;
      }

      Vitality &vitality() {
        return vitality_;
      }

      const Vitality &vitality() const {
        return vitality_;
      }

      const LockerHiding &hiding() const {
        return hiding_;
      }

      PlayerActivityEnum activity() const {
        return activity_;
      }

      Vec2 aimDirection() const {
        return aim_direction_;
      }

      bool isInteracting() const {
        return is_interacting_;
      }

      bool isDead() const {
        return vitality_.isDead();
      }

      bool isHidden() const {
        return activity_ == PlayerActivityEnum::hiding;
      }

      bool isRepairing() const {
        return activity_ == PlayerActivityEnum::repairing;
      }

      bool canBeTargeted() const {
        return !isDead() && !isHidden();
      }

      float sightRangeForMobs() const;

    private:
      static constexpr float aura_radius   = 54;
      static constexpr float aim_turn_rate = 30;

      FlashlightGripDto grip() const;
      void              emitLights(GameContextDto &context) const;
      void              aimAtMouse(GameContextDto &context);
      void              faceRepairTarget();
      void              explore(
        GameContextDto    &context,
        PlayerActivityEnum previous_activity
      );
      void handleExplorationActions(GameContextDto &context);
      void updateFlashlight(GameContextDto &context);

      Vec2                aim_direction_ = Vec2::of(1, 0);
      PlayerActivityEnum  activity_      = PlayerActivityEnum::exploring;
      std::optional<Vec2> repair_target_;
      bool                is_interacting_ = false;
      Flashlight          flashlight_;
      Inventory           inventory_;
      Vitality            vitality_;
      LockerHiding        hiding_;
      PlayerMovement      movement_;
      ThrowingArm         throwing_arm_;
      HugController       hug_controller_;
  };
} // namespace wheel_of_fear
