#pragma once

#include <cstdint>
#include "stm32f4xx.h"

namespace Random
{
    uint32_t GetUint32();
    int32_t GetInt32();
    uint32_t GetUint32(uint32_t min, uint32_t max);
    int32_t GetInt32(int32_t min, int32_t max);
}
