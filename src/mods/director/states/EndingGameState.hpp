#pragma once

#include "mods/credits/CreditLineProps.hpp"
#include "mods/director/states/GameState.hpp"
#include <vector>

namespace wheel_of_fear {
  class EndingGameState final: public GameState {
    public:
      EndingGameState();
      void mixMusic(GameContextDto &context) const override;

      void update(
        Director       &director,
        GameContextDto &context
      ) override;

      void draw(
        const Director &director,
        GameContextDto &context
      ) const override;

    private:
      static constexpr float credits_delay_seconds    = 5;
      static constexpr float scroll_pixels_per_second = 32;
      static constexpr float skip_unlock_seconds      = 1.5F;
      static constexpr float backdrop_fade_seconds    = 2;
      static constexpr float backdrop_alpha           = 0.45F;

      [[nodiscard]]
      static float rollScroll(float state_seconds);

      [[nodiscard]]
      bool isRollFinished(float state_seconds) const;

      std::vector<CreditLineProps> lines_;
      float                        roll_height_ = 0;
  };
} // namespace wheel_of_fear
