#pragma once

#include <array>
#include <cstdint>

namespace wheel_of_fear {
  enum class RoomLayoutEnum : std::uint8_t {
    hall,
    offices,
    server_farm,
    storage,
    pump_station,
  };

  inline constexpr std::array<RoomLayoutEnum, 5> all_room_layouts {
    RoomLayoutEnum::hall,
    RoomLayoutEnum::offices,
    RoomLayoutEnum::server_farm,
    RoomLayoutEnum::storage,
    RoomLayoutEnum::pump_station,
  };
} // namespace wheel_of_fear
