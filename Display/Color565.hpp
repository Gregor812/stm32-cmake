#pragma once

#include <cstdint>

enum class Color565 : uint16_t
{
    Black = 0x0000,
    White = 0xFFFF,
    Red = 0xF800,
    Green = 0x07E0,
    Blue = 0x001F,
    Yellow = Red + Green,
    Cyan = Green + Blue,
    Magenta = Red + Blue
};
