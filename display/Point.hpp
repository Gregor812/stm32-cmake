#pragma once

#include <cstdint>
#include <utility>

struct Point
{
    uint16_t X;
    uint16_t Y;

    void Swap(Point & other)
    {
        std::swap(X, other.X);
        std::swap(Y, other.Y);
    }
};
