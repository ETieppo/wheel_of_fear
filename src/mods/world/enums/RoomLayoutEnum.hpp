#pragma once

#include <array>

namespace wheel_of_fear {

enum class RoomLayoutEnum { hall, offices, server_farm, storage, pump_station };

inline constexpr std::array<RoomLayoutEnum, 5> all_room_layouts{
    RoomLayoutEnum::hall,
    RoomLayoutEnum::offices,
    RoomLayoutEnum::server_farm,
    RoomLayoutEnum::storage,
    RoomLayoutEnum::pump_station,
};

}
