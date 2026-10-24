#pragma once

#include "core/entity/Entity.hpp"
#include "game/dtos/GameContextDto.hpp"
#include "mods/mobs/MobTraitsProps.hpp"
#include "mods/mobs/components/MobAppearanceComponent.hpp"
#include "mods/mobs/components/MobBrainComponent.hpp"
#include "mods/mobs/components/MobConditionComponent.hpp"
#include "mods/mobs/components/MobLocomotionComponent.hpp"
#include "mods/mobs/components/MobPerceptionComponent.hpp"
#include "mods/mobs/components/MobVoiceComponent.hpp"
#include "mods/mobs/enums/MobKindEnum.hpp"

namespace wheel_of_fear {
  class MobEntity: public Entity {
    public:
      MobEntity(
        Vec2Props             position,
        const MobTraitsProps &traits
      );
      MobEntity(
        const MobEntity &
      ) = delete;
      MobEntity &operator =(
        const MobEntity &
      ) = delete;

      [[nodiscard]]
      virtual MobKindEnum kind() const = 0;
      [[nodiscard]]
      virtual const MobTraitsProps &traits() const = 0;

      virtual void think(
        GameContextDto &context
      ) {
        brain_.hunt(context);
      }

      virtual void onContact(
        GameContextDto &context
      );

      virtual void onLightReleased(
        [[maybe_unused]] GameContextDto &context
      ) {
      }

      virtual void emitLights(
        [[maybe_unused]] GameContextDto &context
      ) {
      }

      virtual void draw(
        GameContextDto &context
      ) const = 0;

      [[nodiscard]]
      virtual bool isDisguised() const {
        return false;
      }

      virtual void ignite(
        GameContextDto &context
      );
      virtual void catchBlast(
        GameContextDto &context
      );

      virtual void onGeneratorOnline(
        [[maybe_unused]] GameContextDto &context
      ) {
      }

      void update(
        GameContextDto &context
      );

      [[nodiscard]]
      bool isBusy() const {
        return !isAlive() ||
               condition_.isBeingHugged() ||
               condition_.isCrumbling() ||
               condition_.isBurning();
      }

      [[nodiscard]]
      bool canAct() const {
        return !isBusy() && !condition_.isFrozen();
      }

      [[nodiscard]]
      float incidentCooldown() const {
        return incident_cooldown_;
      }

      void setIncidentCooldown(
        float seconds
      ) {
        incident_cooldown_ = seconds;
      }

      [[nodiscard]]
      float distanceToPlayer(
        const GameContextDto &context
      ) const;

      [[nodiscard]]
      MobBrainComponent &brain() {
        return brain_;
      }

      [[nodiscard]]
      const MobBrainComponent &brain() const {
        return brain_;
      }

      [[nodiscard]]
      MobPerceptionComponent &perception() {
        return perception_;
      }

      [[nodiscard]]
      const MobPerceptionComponent &perception() const {
        return perception_;
      }

      [[nodiscard]]
      MobLocomotionComponent &locomotion() {
        return locomotion_;
      }

      [[nodiscard]]
      const MobLocomotionComponent &locomotion() const {
        return locomotion_;
      }

      [[nodiscard]]
      MobConditionComponent &condition() {
        return condition_;
      }

      [[nodiscard]]
      const MobConditionComponent &condition() const {
        return condition_;
      }

      [[nodiscard]]
      MobVoiceComponent &voice() {
        return voice_;
      }

      [[nodiscard]]
      const MobVoiceComponent &voice() const {
        return voice_;
      }

      [[nodiscard]]
      MobAppearanceComponent &appearance() {
        return appearance_;
      }

      [[nodiscard]]
      const MobAppearanceComponent &appearance() const {
        return appearance_;
      }

    private:
      static constexpr float default_retreat_seconds = 1.2F;

      MobBrainComponent      brain_;
      MobPerceptionComponent perception_;
      MobLocomotionComponent locomotion_;
      MobConditionComponent  condition_;
      MobVoiceComponent      voice_;
      MobAppearanceComponent appearance_;
      float                  incident_cooldown_ = 0;
  };
} // namespace wheel_of_fear
