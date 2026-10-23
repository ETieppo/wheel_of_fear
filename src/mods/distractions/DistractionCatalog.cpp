#include "mods/distractions/DistractionCatalog.hpp"

#include "core/theme/Palette.hpp"

namespace wheel_of_fear {
  DistractionTraitsProps
  DistractionCatalog::traitsOf(DistractionKindEnum kind) {
    switch(kind) {
      case DistractionKindEnum::metal_clang:
        return DistractionTraitsProps {
          SoundEnum::metal_clang, 1.0F, 1, 230, std::nullopt
        };
      case DistractionKindEnum::pipe_rattle:
        return DistractionTraitsProps {
          SoundEnum::metal_clang, 0.7F, 0.6F, 170, std::nullopt
        };
      case DistractionKindEnum::mob_fight:
        return DistractionTraitsProps {
          SoundEnum::distant_scream, 0.9F, 1, 260, std::nullopt
        };
      case DistractionKindEnum::glass_shatter:
        return DistractionTraitsProps {
          SoundEnum::glass_shatter,
          0.9F,
          1,
          200,
          Palette::neon_cyan,
          60,
          0.25F,
          8
        };
      case DistractionKindEnum::power_surge:
        return DistractionTraitsProps {
          SoundEnum::electric_buzz,
          1.0F,
          1,
          220,
          Color::hex(0xe8f4ff),
          150,
          0.6F,
          20
        };
    }
    return DistractionTraitsProps {};
  }
} // namespace wheel_of_fear
