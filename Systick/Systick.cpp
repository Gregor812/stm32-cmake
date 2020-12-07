#include "stm32f4xx.h"

namespace Systick
{
    static volatile uint32_t sysTickCounter = 0;

    void Initialize()
    {        
        SysTick_Config(168000000L/1000L);
        SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
        NVIC_SetPriority(SysTick_IRQn, 0);
    }

    void DelayMilliseconds(uint32_t milliseconds)
    {
        uint32_t counterSnapshot = sysTickCounter;
        while (sysTickCounter - counterSnapshot < milliseconds)
        {}
    }
}

extern "C" void SysTick_Handler()
{
    Systick::sysTickCounter++;
}
