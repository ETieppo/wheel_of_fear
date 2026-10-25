#pragma once

#include "core/audio/enums/SoundEnum.hpp"
#include "mods/lighting/enums/LightFilterEnum.hpp"
#include <cstdint>
#include <optional>
#include <string_view>

namespace wheel_of_fear {
  struct MobTraitsProps {
      std::string_view               display_name;
      std::optional<LightFilterEnum> frozen_by;
      std::optional<SoundEnum>       movement_sound;
      std::optional<SoundEnum>       vocal_sound;
      std::uint32_t                  first_phase      = 1;
      float                          speed            = 0;
      float                          collision_radius = 0;
      LightFilterEnum                visible_under = LightFilterEnum::any_light;
      float                          sight_factor  = 1;
      float                          field_of_view_cosine    = 0.2F;
      float                          hearing_factor          = 1;
      float                          strike_reach            = 10;
      float                          movement_sound_volume   = 0.35F;
      float                          movement_sound_interval = 0.6F;
      float                          vocal_volume            = 0.6F;
      bool                           is_hostile              = true;
      bool                           is_huggable             = true;
      bool                           can_burn                = true;
      bool                           avoids_hazards          = true;
      bool                           passes_through_walls    = false;
      float                          spawn_weight            = 0;
      bool                           has_body_collision      = false;

      [[nodiscard]]
      constexpr bool canSee() const {
        return sight_factor > 0;
      }
  };
} // namespace wheel_of_fear
