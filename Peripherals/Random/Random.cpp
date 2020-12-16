#include "Random.hpp"
#include "stm32f4xx.h"

namespace Peripherals
{
    Random::Random()
    {
        RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
        RNG->CR |= RNG_CR_RNGEN;
    }
    
    uint32_t Random::GetUint32()
    {
        return Instance().GetUint32Inst();
    }

    uint32_t Random::GetUint32(uint32_t min, uint32_t max)
    {
        return Instance().GetUint32Inst(min, max);
    }

    Random& Random::Instance(void)
    {
        static Random instance;
        return instance;
    }

    uint32_t Random::GetValue()
    {
        while (!(RNG->SR & RNG_SR_DRDY));
        return RNG->DR;
    }

    uint32_t Random::GetUint32Inst()
    {
        return GetValue();
    }

    uint32_t Random::GetUint32Inst(uint32_t min, uint32_t max)
    {
        if (max < min) return 0;
        if (max == min) return max;
        return (GetValue() % (max - min)) + min;
    }
}
