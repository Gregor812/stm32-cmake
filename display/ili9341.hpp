#pragma once

#include <cstdint>
#include <utility>

#include "stm32f4xx.h"
#include "systick.hpp"

enum class PhysicalInterface
{
    Serial8Bit4Wire = 0x6
};

enum class ColorMode
{
    R5G6B5,
    R6G6B6
};

enum class Color565 : uint16_t
{
    Black = 0x0000,
    White = 0xFFFF,
    Red = 0xF800,
    Green = 0x07E0,
    Blue = 0x001F,
    Yellow = Red + Green,
    Cyan = Green + Blue,
    Magenta = Red + Blue
};

enum class Command : uint8_t
{
    Reset = 0x01,
    SleepOut = 0x11,
    // GammaSet = 0x26,
    // DisplayOff = 0x28,
    DisplayOn = 0x29,
    ColumnAddressSet = 0x2A,
    PageAddressSet = 0x2B,
    MemoryWrite = 0x2C,
    MemoryAccessControl = 0x36,
    PixelFormatSet = 0x3A,
    // WriteDisplayBrightness = 0x51,
    // WriteCTRLDisplay = 0x53,
    // RgbInterfaceSignalControl = 0xB0,
    // FrameControlNormalMode = 0xB1,
    // BlankingPorchControl = 0xB5,
    // DisplayFunctionControl = 0xB6,
    // PowerControl1 = 0xC0,
    // PowerControl2 = 0xC1,
    // VComControl1 = 0xC5,
    // VComControl2 = 0xC7,
    // PowerControlA = 0xCB,
    // PowerControlB = 0xCF,
    // PositiveGammaCorrection = 0xE0,
    // NegativeGammaCorrection = 0xE1,
    // DriverTimingControlA = 0xE8,
    // DriverTimingControlB = 0xEA,
    // PowerOnSequenceControl = 0xED,
    // Enable3GammaControl = 0xF2,
    // InterfaceControl = 0xF6,
    // PumpRatioControl = 0xF7
};

class Ili9341
{
private:
    enum class WriteMode
    {
        Command = 0,
        Data = 1
    };

public:
    Ili9341(Ili9341 &&other)
    {
        Swap(other);
    }

    Ili9341 &operator=(Ili9341 &&other)
    {
        Swap(other);
        return *this;
    }

    static Ili9341 ForSerial8Bit4Wire(SPI_TypeDef *spi,
        GPIO_TypeDef *chipSelectPort, uint8_t chipSelectPin,
        GPIO_TypeDef *_dataCommandSelectPort, uint8_t _dataCommandSelectPin,
        GPIO_TypeDef *spiPort, uint8_t spiClockPin, uint8_t spiIoPin,
        uint16_t originX, uint8_t originY,
        uint16_t width, uint8_t height,
        ColorMode colorMode)
    {
        Ili9341 result;
        result._spi = spi;
        result._chipSelectPort = chipSelectPort;
        result._dataCommandSelectPort = _dataCommandSelectPort;
        result._spiPort = spiPort;
        result._chipSelectPin = chipSelectPin;
        result._dataCommandSelectPin = _dataCommandSelectPin;
        result._spiClockPin = spiClockPin;
        result._spiIoPin = spiIoPin;
        result._originX = originX;
        result._originY = originY;
        result._width = width;
        result._height = height;
        result._colorMode = colorMode;
        result._physicalInterface = PhysicalInterface::Serial8Bit4Wire;
        return result;
    }

    void Init() const
    {
        _dataCommandSelectPort->MODER |= (1 << (_dataCommandSelectPin << 1));
        _dataCommandSelectPort->OSPEEDR |= (3 << (_dataCommandSelectPin << 1));

        _chipSelectPort->BSRR = (1 << _chipSelectPin);
        _chipSelectPort->MODER |= (1 << (_chipSelectPin << 1));
        _chipSelectPort->OSPEEDR |= (3 << (_chipSelectPin << 1));

        _spiPort->MODER |= (2 << (_spiClockPin << 1)) | (2 << (_spiIoPin << 1));
        _spiPort->OSPEEDR |= (3 << (_spiClockPin << 1)) | (3 << (_spiIoPin << 1));
        _spiPort->AFR[0] |= (5 << GPIO_AFRL_AFSEL7_Pos);
        _spiPort->AFR[1] |= (5 << GPIO_AFRH_AFSEL9_Pos);

        _spi->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE;
        _chipSelectPort->BSRR = (1 << (_chipSelectPin + 16));
        WriteCommand(Command::Reset);
        Systick::DelayMilliseconds(15);
        
        uint8_t highByte = _originX >> 8;
        uint8_t lowByte = _originX;
        WriteCommand(Command::ColumnAddressSet);
        WriteData(highByte);
        WriteData(lowByte);
        highByte = (_originX + _width - 1) >> 8;
        lowByte = (_originX + _width - 1);
        WriteData(highByte);
        WriteData(lowByte);
        Systick::DelayMilliseconds(15);

        WriteCommand(Command::PageAddressSet);
        WriteData(0);
        WriteData(_originY);
        WriteData(0);
        WriteData(_originY + _height - 1);
        Systick::DelayMilliseconds(15);

        WriteCommand(Command::PixelFormatSet);
        switch (_colorMode)
        {
            case ColorMode::R5G6B5: 
                WriteData(0x55);
                break;
            case ColorMode::R6G6B6:
                WriteData(0x66);
                break;
        }
        Systick::DelayMilliseconds(15);
        
        WriteCommand(Command::MemoryAccessControl);
        WriteData(0x28);
        Systick::DelayMilliseconds(15);

        WriteCommand(Command::DisplayOn);
        Systick::DelayMilliseconds(15);

        WriteCommand(Command::SleepOut);
        Systick::DelayMilliseconds(15);
        
        _chipSelectPort->BSRR = (1 << _chipSelectPin);
    }

    void DrawRussia()
    {
        _chipSelectPort->BSRR = (1 << (_chipSelectPin + 16));
        WriteCommand(Command::MemoryWrite);
        int i;
        for (i = 0; i < _height / 3; ++i)
        {
            for (int j = 0; j < _width; ++j)
            {
                WriteColor(Color565::White);
            }
        }
        for (; i < 2 * _height / 3; ++i)
        {
            for (int j = 0; j < _width; ++j)
            {
                WriteColor(Color565::Blue);
            }
        }
        for (; i < _height; ++i)
        {
            for (int j = 0; j < _width; ++j)
            {
                WriteColor(Color565::Red);
            }
        }
        
        Systick::DelayMilliseconds(15);
        _chipSelectPort->BSRR = (1 << _chipSelectPin);
    }

    void DrawGermany()
    {
        _chipSelectPort->BSRR = (1 << (_chipSelectPin + 16));
        WriteCommand(Command::MemoryWrite);
        int i;
        for (i = 0; i < _height / 3; ++i)
        {
            for (int j = 0; j < _width; ++j)
            {
                WriteColor(Color565::Black);
            }
        }
        for (; i < 2 * _height / 3; ++i)
        {
            for (int j = 0; j < _width; ++j)
            {
                WriteColor(Color565::Red);
            }
        }
        for (; i < _height; ++i)
        {
            for (int j = 0; j < _width; ++j)
            {
                WriteColor(Color565::Yellow);
            }
        }
        
        Systick::DelayMilliseconds(15);
        _chipSelectPort->BSRR = (1 << _chipSelectPin);
    }

    void DrawUkraine()
    {
        _chipSelectPort->BSRR = (1 << (_chipSelectPin + 16));
        WriteCommand(Command::MemoryWrite);
        int i;
        for (i = 0; i < _height / 2; ++i)
        {
            for (int j = 0; j < _width; ++j)
            {
                WriteColor(Color565::Blue);
            }
        }
        for (; i < _height; ++i)
        {
            for (int j = 0; j < _width; ++j)
            {
                WriteColor(Color565::Yellow);
            }
        }
        
        Systick::DelayMilliseconds(15);
        _chipSelectPort->BSRR = (1 << _chipSelectPin);
    }

    void WriteCommand(uint8_t command) const
    {
        Write(command, WriteMode::Command);
    }

    void WriteCommand(Command command) const
    {
        Write(static_cast<uint8_t>(command), WriteMode::Command);
    }

    void WriteColor(Color565 color)
    {
        if (_colorMode != ColorMode::R5G6B5)
            return;

        uint16_t data = static_cast<uint16_t>(color);
        uint8_t highByte = data >> 8;
        uint8_t lowByte = data;
        WriteData(highByte);
        WriteData(lowByte);
    }

    void WriteData(uint8_t data) const
    {
        Write(data, WriteMode::Data);
    }

    PhysicalInterface GetPhysicalInterface() const
    {
        return _physicalInterface;
    }

    Ili9341(Ili9341 const &) = delete;
    Ili9341 &operator=(Ili9341 const &) = delete;

private:

    Ili9341() = default;

    void Swap(Ili9341 &other)
    {
        std::swap(_spi, other._spi);

        std::swap(_chipSelectPort, other._chipSelectPort);
        std::swap(_dataCommandSelectPort, other._dataCommandSelectPort);
        std::swap(_spiPort, other._spiPort);

        std::swap(_chipSelectPin, other._chipSelectPin);
        std::swap(_dataCommandSelectPin, other._dataCommandSelectPin);
        std::swap(_spiClockPin, other._spiClockPin);
        std::swap(_spiIoPin, other._spiIoPin);

        std::swap(_originX, other._originX);
        std::swap(_originY, other._originY);
        std::swap(_width, other._width);
        std::swap(_height, other._height);

        std::swap(_colorMode, other._colorMode);

        std::swap(_physicalInterface, other._physicalInterface);
    }

    void Write(uint8_t data, WriteMode writeMode) const
    {
        while(_spi->SR & SPI_SR_BSY)
        {}
        if (writeMode == WriteMode::Command)
            _dataCommandSelectPort->BSRR = (1 << (_dataCommandSelectPin + 16));
        else
            _dataCommandSelectPort->BSRR = (1 << _dataCommandSelectPin);

        while ((_spi->SR & SPI_SR_TXE) == 0)
        {}
        _spi->DR = data;

        if (writeMode == WriteMode::Command)
            Systick::DelayMicrosecondsResolution100us(100);
    }

    SPI_TypeDef *_spi;

    GPIO_TypeDef *_chipSelectPort;
    GPIO_TypeDef *_dataCommandSelectPort;
    GPIO_TypeDef *_spiPort;
    
    uint8_t _chipSelectPin;
    uint8_t _dataCommandSelectPin;
    uint8_t _spiClockPin;
    uint8_t _spiIoPin;

    uint16_t _originX;
    uint8_t _originY;
    uint16_t _width;
    uint8_t _height;

    ColorMode _colorMode;

    PhysicalInterface _physicalInterface = PhysicalInterface::Serial8Bit4Wire;
};
