#pragma once

#include "mods/credits/CreditEntryProps.hpp"
#include "mods/director/states/GameState.hpp"
#include <vector>

namespace wheel_of_fear {
  class CreditsGameState final: public GameState {
    public:
      CreditsGameState();
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
      std::vector<CreditEntryProps> entries_;
  };
} // namespace wheel_of_fear
