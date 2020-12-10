#pragma once

#include <functional>

namespace Display
{
    class IDisplay
    {
    public:

        virtual void InitializeHardware(void) = 0;
        virtual void InitializeSoftware(void) = 0;
        virtual void RedrawFrame(void) = 0;
        virtual ~IDisplay()
        {}
    };
}
