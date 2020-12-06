#pragma once

#include <cstdint>

class IOutput
{
public:
    virtual void Write(uint8_t *bufferStart, uint8_t *bufferEnd) = 0;
    virtual bool IsBusy() = 0;
    virtual ~IOutput()
    {}
};
