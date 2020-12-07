#pragma once

#include <functional>
#include "IDisplay.hpp"
#include "stm32f4xx.h"
#include "Systick.hpp"
#include "Spi.hpp"

namespace Ili9341
{
    class SpiDriver : public Display::IDisplay
    {
    public:

        SpiDriver(SPI_TypeDef *spi,
            std::function<void (SPI_TypeDef *)> initialize)
            : _spi(Peripherals::Spi8Bit(spi, initialize))
        {}

        SpiDriver& InitializeHardware(void) override
        {
            _spi = _spi.Initialize()
                .ActivateChipSelect();
            return *this;
        }

        SpiDriver& InitializeSoftware(void) override
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

            return *this;
        }

        void RedrawFrame(void) override
        {
            for(int i = 0; i < 320 * 240 * 2; ++i)
            {
                WriteData(54);
                WriteData(98);
            }
        }

        ~SpiDriver() override
        {
            //_spi.ReleaseChipSelect();
        }

    private:

        void SetConnectionMode(void)
        {
            GPIOD->BSRR = (1 << 4) | (1 << 5);
            GPIOD->BSRR = (1 << (2 + 16)) | (1 << (7 + 16));
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
            GPIOD->BSRR = (1 << (13 + 16));
            
            _spi.Write(command);
            
            while(!_spi.BufferIsEmpty());
            while(_spi.IsBusy());
            GPIOD->BSRR = (1 << 13);
        }

        void WriteData(uint8_t data)
        {
            while(!_spi.BufferIsEmpty());
            _spi.Write(data);
        }


        Peripherals::Spi8Bit _spi;
    };
}
