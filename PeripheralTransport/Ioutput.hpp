#pragma once

#include <cstdint>

class IOutput
{
public:
    virtual void Write(uint8_t *bufferStart, uint8_t *bufferEnd);
    virtual bool IsBusy();
};
