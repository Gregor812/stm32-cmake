#pragma once

#include <functional>

namespace Display
{
    class IDisplay
    {
    public:

        virtual IDisplay& InitializeHardware(void) = 0;
        virtual IDisplay& InitializeSoftware(void) = 0;
        virtual void RedrawFrame(void) = 0;
        virtual ~IDisplay()
        {}
    };
}
