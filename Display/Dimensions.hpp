#pragma once

#include <cstdint>
#include <utility>

struct Dimensions
{
    uint16_t Width;
    uint16_t Height;

    void Swap(Dimensions & other)
    {
        std::swap(Width, other.Width);
        std::swap(Height, other.Height);
    }
};
