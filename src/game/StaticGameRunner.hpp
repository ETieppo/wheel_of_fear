#pragma once

#include "game/GameApplication.hpp"
#include "game/GameRunner.hpp"
#include <string_view>

namespace wheel_of_fear {
  class StaticGameRunner final: public GameRunner {
    public:
      explicit StaticGameRunner(Window &window);
      bool runFrame() override;

    private:
      static constexpr std::string_view progress_file_name = "progress.sav";
      static constexpr std::string_view settings_file_name = "settings.cfg";

      GameApplication game_;
  };
} // namespace wheel_of_fear
