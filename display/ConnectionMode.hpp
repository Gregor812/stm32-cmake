#pragma once

#include <cstdint>

enum class ConnectionMode : uint8_t
{
    Parallel8BitIntel = 0x0,
    Parallel16BitIntel = 0x1,
    Parallel9BitIntel = 0x2,
    Parallel18BitIntel = 0x3,
    Serial8Bit4Wire = 0x6
};
