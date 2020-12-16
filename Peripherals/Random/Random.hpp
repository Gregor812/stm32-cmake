#pragma once

#include <cstdint>

namespace Peripherals
{
    class Random
    {
    public:
        static uint32_t GetUint32();
        static uint32_t GetUint32(uint32_t min, uint32_t max);

        Random(const Random&) = delete;
        Random& operator=(const Random&) = delete;
        Random(const Random&&) = delete;
        Random& operator=(const Random&&) = delete;
    private:
        Random();

        uint32_t GetUint32Inst();
        uint32_t GetUint32Inst(uint32_t min, uint32_t max);
        static uint32_t GetValue();

        static Random& Instance(void);
    };
}
