#pragma once

#include "mods/director/states/GameState.hpp"

namespace wheel_of_fear {
  class TitleGameState final: public GameState {
    public:
      void update(
        Director       &director,
        GameContextDto &context
      ) override;
      void mixMusic(
        GameContextDto &context
      ) const override;
      void draw(
        const Director &director,
        GameContextDto &context
      ) const override;

    private:
      static void panCamera(
        const Director &director,
        GameContextDto &context
      );
  };
} // namespace wheel_of_fear
