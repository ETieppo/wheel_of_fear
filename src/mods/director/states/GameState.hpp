#pragma once

#include "game/dtos/GameContextDto.hpp"

namespace wheel_of_fear {
  class Director;

  class GameState {
    public:
      GameState()                                          = default;
      GameState(const GameState &)                         = delete;
      GameState &operator =(const GameState &)             = delete;
      virtual ~GameState()                                 = default;
      virtual void mixMusic(GameContextDto &context) const = 0;

      virtual void update(
        Director       &director,
        GameContextDto &context
      ) = 0;

      virtual void draw(
        [[maybe_unused]] const Director &director,
        [[maybe_unused]] GameContextDto &context
      ) const {
      }
  };
} // namespace wheel_of_fear
