#include <cstdint>

namespace Systick
{
    static volatile uint32_t sysTickCounter = 0;

    void DelayMilliseconds(uint32_t milliseconds)
    {
        uint32_t counterSnapshot = sysTickCounter;
        while (sysTickCounter - counterSnapshot < milliseconds * 10)
        {}
    }

    void DelayMicrosecondsResolution100us(uint32_t microseconds)
    {
        uint32_t counterSnapshot = sysTickCounter;
        while (sysTickCounter - counterSnapshot < microseconds / 100)
        {}
    }
} // namespace systick

extern "C" void SysTick_Handler()
{
    Systick::sysTickCounter++;
}
