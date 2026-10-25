#pragma once

#include "mods/director/states/GameState.hpp"

namespace wheel_of_fear {
  class ReadingGameState final: public GameState {
    public:
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
      static constexpr float close_unlock_seconds = 0.4F;
  };
} // namespace wheel_of_fear
