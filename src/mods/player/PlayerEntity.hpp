#pragma once

#include "core/entity/Entity.hpp"
#include "core/math/Vec2Props.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/flashlight/FlashlightComponent.hpp"
#include "mods/player/components/HugController.hpp"
#include "mods/player/components/InventoryComponent.hpp"
#include "mods/player/components/LockerHidingComponent.hpp"
#include "mods/player/components/PlayerAimComponent.hpp"
#include "mods/player/components/PlayerMovementComponent.hpp"
#include "mods/player/components/ThrowingArmComponent.hpp"
#include "mods/player/components/VitalityComponent.hpp"
#include "mods/player/enums/PlayerActivityEnum.hpp"
#include <cstddef>
#include <optional>

namespace wheel_of_fear {
  class PlayerEntity final: public Entity {
    public:
      static constexpr float collision_half_size = 4.5F;

      PlayerEntity();

      void respawn(Vec2Props position);

      void update(GameContextDto &context);

      void draw(GameContextDto &context) const;

      void beginRepair(Vec2Props target);

      void takeHit(
        GameContextDto &context,
        Vec2Props       source
      );

      void dragOutOfLocker(GameContextDto &context);

      FlashlightComponent &flashlight() {
        return flashlight_;
      }

      [[nodiscard]]
      const FlashlightComponent &flashlight() const {
        return flashlight_;
      }

      InventoryComponent &inventory() {
        return inventory_;
      }

      [[nodiscard]]
      const InventoryComponent &inventory() const {
        return inventory_;
      }

      VitalityComponent &vitality() {
        return vitality_;
      }

      [[nodiscard]]
      const VitalityComponent &vitality() const {
        return vitality_;
      }

      [[nodiscard]]
      const LockerHidingComponent &hiding() const {
        return hiding_;
      }

      [[nodiscard]]
      PlayerActivityEnum activity() const {
        return activity_;
      }

      [[nodiscard]]
      Vec2Props aimDirection() const {
        return aim_.direction();
      }

      [[nodiscard]]
      bool isInteracting() const {
        return is_interacting_;
      }

      [[nodiscard]]
      bool isDead() const {
        return vitality_.isDead();
      }

      [[nodiscard]]
      bool isHidden() const {
        return activity_ == PlayerActivityEnum::hiding;
      }

      [[nodiscard]]
      bool isRepairing() const {
        return activity_ == PlayerActivityEnum::repairing;
      }

      [[nodiscard]]
      bool canBeTargeted() const {
        return !isDead() && !isHidden();
      }

      [[nodiscard]]
      float sightRangeForMobs() const;

    private:
      static constexpr float aura_radius   = 54;
      static constexpr float aim_turn_rate = 30;

      void turnTowardAim();
      void faceRepairTarget();

      [[nodiscard]]
      FlashlightGripDto grip() const;

      void emitLights(GameContextDto &context) const;

      void explore(
        GameContextDto    &context,
        PlayerActivityEnum previous_activity
      );

      void handleExplorationActions(GameContextDto &context);

      void updateFlashlight(GameContextDto &context);

      std::optional<Vec2Props> repair_target_;
      PlayerActivityEnum       activity_       = PlayerActivityEnum::exploring;
      bool                     is_interacting_ = false;
      PlayerAimComponent       aim_;
      FlashlightComponent      flashlight_;
      InventoryComponent       inventory_;
      VitalityComponent        vitality_;
      LockerHidingComponent    hiding_;
      PlayerMovementComponent  movement_;
      ThrowingArmComponent     throwing_arm_;
  };
} // namespace wheel_of_fear
