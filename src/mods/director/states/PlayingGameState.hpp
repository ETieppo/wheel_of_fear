#pragma once

#include "mods/director/states/GameState.hpp"

namespace wheel_of_fear {
  class PlayingGameState final: public GameState {
    public:
      void mixMusic(GameContextDto &context) const override;

      void update(
        Director       &director,
        GameContextDto &context
      ) override;
  };
} // namespace wheel_of_fear
