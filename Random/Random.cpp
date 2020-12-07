#include "Random.hpp"
#include "stm32f4xx.h"

namespace Random
{
    static uint32_t GetValue();
    
    void Initialize()
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
        RNG->CR |= RNG_CR_RNGEN;
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

    static uint32_t GetValue()
    {
        while (!(RNG->SR & RNG_SR_DRDY));
        return RNG->DR;
    }
}
