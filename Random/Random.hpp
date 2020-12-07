#pragma once

#include <cstdint>

namespace Random
{
    void Initialize();
    uint32_t GetUint32();
    uint32_t GetUint32(uint32_t min, uint32_t max);
}
