#pragma once

namespace wheel_of_fear {
  class VitalityComponent {
    public:
      static constexpr int max_health = 3;

      void restore();
      bool absorbHit();

      [[nodiscard]]
      int health() const {
        return health_;
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

      int   health_               = max_health;
      float invulnerable_seconds_ = spawn_invulnerability_seconds;
      float hurt_flash_           = 0;
      bool  is_dead_              = false;
  };
} // namespace wheel_of_fear
