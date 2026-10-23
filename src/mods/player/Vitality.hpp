#pragma once

namespace wheel_of_fear {
  class Vitality {
    public:
      static constexpr int max_health = 3;

      int health() const {
        return health_;
      }

      bool isDead() const {
        return is_dead_;
      }

      bool isInvulnerable() const {
        return invulnerable_seconds_ > 0;
      }

      float hurtFlash() const {
        return hurt_flash_;
      }

      void restore();
      void tick(float delta_seconds);
      bool absorbHit();

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
