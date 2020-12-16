#include "stm32f4xx.h"
#include "Systick.hpp"

namespace Core
{
    Systick::Systick()
    {
        sysTickCounter = 0;
        SysTick_Config(168000000L/1000L);
        SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
        NVIC_SetPriority(SysTick_IRQn, 0);
    }

    Systick& Systick::Instance(void)
    {
        static Systick instance;
        return instance;
    }

    void Systick::DelayMilliseconds(uint32_t milliseconds)
    {
        Instance().DelayMillisecondsInst(milliseconds);
    }

    void Systick::DelayMillisecondsInst(uint32_t milliseconds)
    {
        uint32_t counterSnapshot = this->sysTickCounter;
        while (sysTickCounter - counterSnapshot < milliseconds)
        {}
    }

    void SysTickHandler()
    {
        Systick::Instance().sysTickCounter++;
    }
}

extern "C" void SysTick_Handler()
{
    Core::SysTickHandler();
}
