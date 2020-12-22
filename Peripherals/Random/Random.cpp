#include "Random.hpp"

#ifdef STM32F429xx
    #include "Impl/RandomImplStF429.hpp"
#endif

namespace Peripherals
{
    uint32_t Random::GetUint32()
    {
        return RandomImpl::GetUint32();
    }

    uint32_t Random::GetUint32(uint32_t min, uint32_t max)
    {
        return RandomImpl::GetUint32(min, max);
    }
}
