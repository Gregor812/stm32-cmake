#pragma once

#include <cstdint>

namespace Core
{
    class Systick
    {
    public:
        static void DelayMilliseconds(uint32_t milliseconds);

        Systick(const Systick&) = delete;
        Systick& operator=(const Systick&) = delete;
        Systick(const Systick&&) = delete;
        Systick& operator=(const Systick&&) = delete;

    private:
        friend void SysTickHandler();
        Systick();

        void DelayMillisecondsInst(uint32_t milliseconds);

        static Systick& Instance(void);
        volatile uint32_t sysTickCounter;
    };
}
