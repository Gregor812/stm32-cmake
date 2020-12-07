#pragma once

#include <cstdint>

namespace Systick
{
    void Initialize();
    void DelayMilliseconds(uint32_t milliseconds);
}
