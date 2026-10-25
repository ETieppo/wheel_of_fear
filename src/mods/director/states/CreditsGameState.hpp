#pragma once

#include "core/text/enums/LanguageEnum.hpp"
#include "mods/credits/CreditEntryProps.hpp"
#include "mods/credits/CreditLineProps.hpp"
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
      static constexpr float scroll_pixels_per_second = 40;
      static constexpr float backdrop_alpha           = 0.88F;
      static constexpr float footer_height            = 46;

      void rebuildLinesIfLanguageChanged();

      std::vector<CreditEntryProps> entries_;
      std::vector<CreditLineProps>  lines_;
      LanguageEnum                  lines_language_ = LanguageEnum::english;
      float                         roll_height_    = 0;
  };
} // namespace wheel_of_fear
