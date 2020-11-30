#include "Random.hpp"

namespace Random
{
    uint32_t GetUint32()
    {
        while(!(RNG->SR & RNG_SR_DRDY));
        return RNG->DR;
    }

    int32_t GetInt32()
    {
        return 0;
    }

    uint32_t GetUint32(uint32_t min, uint32_t max)
    {
        return 0;
    }

    int32_t GetInt32(int32_t min, int32_t max)
    {
        return 0;
    }
} // namespace Random
