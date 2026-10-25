#pragma once

namespace wheel_of_fear {
  class VitalityComponent {
    public:
      static constexpr int base_max_health    = 3;
      static constexpr int highest_max_health = 8;

      void restore();
      void setMaxHealth(int max_health);
      bool grantExtraLife();
      bool absorbHit();

      [[nodiscard]]
      int health() const {
        return health_;
      }

      [[nodiscard]]
      int maxHealth() const {
        return max_health_;
      }

      [[nodiscard]]
      bool isDead() const {
        return is_dead_;
      }

      [[nodiscard]]
      bool isInvulnerable() const {
        return invulnerable_seconds_ > 0;
      }

      [[nodiscard]]
      float hurtFlash() const {
        return hurt_flash_;
      }

      void tick(float delta_seconds);

      void dropGuard() {
        invulnerable_seconds_ = 0;
      }

      void grantInvulnerability(float seconds) {
        invulnerable_seconds_ = seconds;
      }

    private:
      static constexpr float spawn_invulnerability_seconds = 1.5F;
      static constexpr float hit_invulnerability_seconds   = 1.4F;

      int   max_health_           = base_max_health;
      int   health_               = base_max_health;
      float invulnerable_seconds_ = spawn_invulnerability_seconds;
      float hurt_flash_           = 0;
      bool  is_dead_              = false;
  };
} // namespace wheel_of_fear
