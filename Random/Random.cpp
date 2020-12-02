#include "Random.hpp"

namespace Random
{
    static uint32_t GetValue()
    {
        while(!(RNG->SR & RNG_SR_DRDY));
        return RNG->DR;
    }

    uint32_t GetUint32()
    {
        return GetValue();
    }

    uint32_t GetUint32(uint32_t min, uint32_t max)
    {
        if (max < min) return 0;
        if (max == min) return max;
        return (GetValue() % (max - min)) + min;
    }
} // namespace Random
