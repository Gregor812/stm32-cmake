#pragma once

#include <functional>

#include "stm32f4xx.h"
#include "IDisplay.hpp"
#include "Systick.hpp"
#include "Spi.hpp"

using namespace Peripherals;

namespace Ili9341
{
    class SpiDriver : public Display::IDisplay
    {
    public:

        SpiDriver(SPI_TypeDef *spi,
            std::function<void (SPI_TypeDef *)> initialize)
            : _spi(std::move(Spi8Bit(spi, initialize)))
        {}

        void InitializeHardware(void) override
        {
            _spi.Initialize();
            _spi.ActivateChipSelect();
        }

        void InitializeSoftware(void) override
        {
            SetConnectionMode();
            Reset();
            SetColorMode();
            SetOrientation();
            SetSize();

            WriteCommand(0x29);
            Systick::DelayMilliseconds(15);

            WriteCommand(0x11);
            Systick::DelayMilliseconds(15);

            WriteCommand(0x2C);
        }

        void RedrawFrame(void) override
        {
            auto a = Random::GetUint32();
            for(int i = 0; i < 320 * 240 * 2; ++i)
            {
                WriteData(a);
                WriteData(Random::GetUint32());
            }
        }

        ~SpiDriver() override
        {
            _spi.ReleaseChipSelect();
        }

    private:

        void SetConnectionMode(void)
        {
            Gpio::Set<Gpio::Pin::Pin4, Gpio::Pin::Pin5>(GPIOD);
            Gpio::Reset<Gpio::Pin::Pin2, Gpio::Pin::Pin7>(GPIOD);
        }

        void Reset(void)
        {
            WriteCommand(0x01);
            Systick::DelayMilliseconds(1);
        }

        void SetColorMode(void)
        {
            WriteCommand(0x3A);
            WriteData(0x55);
            Systick::DelayMilliseconds(1);
        }

        void SetOrientation(void)
        {
            WriteCommand(0x36);
            WriteData(0x28);
            Systick::DelayMilliseconds(1);
        }

        void SetSize(void)
        {
            WriteCommand(0x2A);
            uint8_t highByte = 0;
            uint8_t lowByte = 0;
            WriteData(highByte);
            WriteData(lowByte);
            highByte = (320 - 1) >> 8;
            lowByte = (320 - 1);
            WriteData(highByte);
            WriteData(lowByte);
            Systick::DelayMilliseconds(1);

            WriteCommand(0x2B);
            highByte = 0;
            lowByte = 0;
            WriteData(highByte);
            WriteData(lowByte);
            highByte = (240 - 1) >> 8;
            lowByte = (240 - 1);
            WriteData(highByte);
            WriteData(lowByte);
            Systick::DelayMilliseconds(1);
        }

        void WriteCommand(uint8_t command)
        {
            while(!_spi.BufferIsEmpty());
            while(_spi.IsBusy());
            Gpio::Reset<Gpio::Pin::Pin13>(GPIOD);
            
            _spi.Write(command);
            
            while(!_spi.BufferIsEmpty());
            while(_spi.IsBusy());
            Gpio::Set<Gpio::Pin::Pin13>(GPIOD);
        }

        void WriteData(uint8_t data)
        {
            while(!_spi.BufferIsEmpty());
            _spi.Write(data);
        }


        Spi8Bit _spi;
    };
}
