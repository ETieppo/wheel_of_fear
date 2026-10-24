#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

#include "core/audio/enums/SoundEnum.hpp"
#include "mods/lighting/enums/LightFilterEnum.hpp"

namespace wheel_of_fear {
  struct MobTraitsProps {
      std::string_view               display_name;
      std::string_view               introduction;
      float                          speed            = 0;
      float                          collision_radius = 0;
      std::optional<LightFilterEnum> frozen_by;
      LightFilterEnum                visible_under = LightFilterEnum::any_light;
      float                          sight_factor  = 1;
      float                          field_of_view_cosine = 0.2F;
      float                          hearing_factor       = 1;
      float                          strike_reach         = 10;
      std::optional<SoundEnum>       movement_sound;
      float                          movement_sound_volume   = 0.35F;
      float                          movement_sound_interval = 0.6F;
      std::optional<SoundEnum>       vocal_sound;
      float                          vocal_volume   = 0.6F;
      bool                           is_hostile     = true;
      bool                           is_huggable    = true;
      bool                           can_burn       = true;
      bool                           avoids_hazards = true;
      std::uint32_t                  first_phase    = 1;
      float                          spawn_weight   = 0;

      [[nodiscard]]
      constexpr bool canSee() const {
        return sight_factor > 0;
      }
  };
} // namespace wheel_of_fear
