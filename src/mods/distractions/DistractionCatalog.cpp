#include "mods/distractions/DistractionCatalog.hpp"

#include "core/theme/Palette.hpp"

namespace wheel_of_fear {
  DistractionTraitsProps
  DistractionCatalog::traitsOf(DistractionKindEnum kind) {
    switch(kind) {
      case DistractionKindEnum::metal_clang:
        return DistractionTraitsProps {
          .sound        = SoundEnum::metal_clang,
          .volume       = 1.0F,
          .pitch        = 1,
          .noise_radius = 230,
          .flash_color  = std::nullopt,
        };

      case DistractionKindEnum::pipe_rattle:
        return DistractionTraitsProps {
          .sound        = SoundEnum::metal_clang,
          .volume       = 0.7F,
          .pitch        = 0.6F,
          .noise_radius = 170,
          .flash_color  = std::nullopt,
        };

      case DistractionKindEnum::mob_fight:
        return DistractionTraitsProps {
          .sound        = SoundEnum::distant_scream,
          .volume       = 0.9F,
          .pitch        = 1,
          .noise_radius = 260,
          .flash_color  = std::nullopt,
        };

      case DistractionKindEnum::glass_shatter:
        return DistractionTraitsProps {
          .sound         = SoundEnum::glass_shatter,
          .volume        = 0.9F,
          .pitch         = 1,
          .noise_radius  = 200,
          .flash_color   = Palette::neon_cyan,
          .flash_radius  = 60,
          .flash_seconds = 0.25F,
          .spark_count   = 8,
        };

      case DistractionKindEnum::power_surge:
        return DistractionTraitsProps {
          .sound         = SoundEnum::electric_buzz,
          .volume        = 1.0F,
          .pitch         = 1,
          .noise_radius  = 220,
          .flash_color   = ColorProps::hex(0xe8f4ff),
          .flash_radius  = 150,
          .flash_seconds = 0.6F,
          .spark_count   = 20,
        };
    }

    return DistractionTraitsProps {};
  }
} // namespace wheel_of_fear
