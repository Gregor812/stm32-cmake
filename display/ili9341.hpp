#pragma once

#include <cstdint>
#include <utility>
#include <cmath>

#include "stm32f4xx.h"
#include "systick.hpp"

#include "ColorMode.hpp"
#include "Orientation.hpp"
#include "Color565.hpp"
#include "Command.hpp"
#include "Point.hpp"
#include "Dimensions.hpp"
#include "Framebuffer.hpp"

template<size_t width, size_t height>
class Ili9341
{
public:

    static Ili9341<width, height> ForSerial8Bit4Wire(
        Framebuffer<width, height> *framebuffer, SPI_TypeDef *spi,
        GPIO_TypeDef *chipSelectPort, uint8_t chipSelectPin,
        GPIO_TypeDef *dataCommandSelectPort, uint8_t dataCommandSelectPin,
        GPIO_TypeDef *connectionModeSelectPort,
        uint8_t (&connectionModeSelectPins)[4],
        Point origin, Dimensions dimensions,
        Orientation orientation)
    {
        Ili9341<width, height> result;
        result._framebuffer = framebuffer;
        result._spi = spi;
        result._chipSelectPort = chipSelectPort;
        result._dataCommandSelectPort = dataCommandSelectPort;
        result._connectionModeSelectPort = connectionModeSelectPort;
        result._chipSelectPin = chipSelectPin;
        result._dataCommandSelectPin = dataCommandSelectPin;
        result._connectionModeSelectPins[0] = connectionModeSelectPins[0];
        result._connectionModeSelectPins[1] = connectionModeSelectPins[1];        
        result._connectionModeSelectPins[2] = connectionModeSelectPins[2];
        result._connectionModeSelectPins[3] = connectionModeSelectPins[3];
        result._origin = origin;
        result._dimensions = dimensions;
        result._orientation = orientation;
        return result;
    }
    
    Ili9341(Ili9341 &&other)
    {
        Swap(other);
    }

    Ili9341 &operator=(Ili9341 &&other)
    {
        Swap(other);
        return *this;
    }

    void Init()
    {
        _chipSelectPort->BSRR = (1 << (_chipSelectPin + 16));
        SetConnectionMode();
        Reset();
        SetColorMode();
        SetOrientation(_orientation);
        SetSize(_origin, _dimensions);

        WriteCommand(Command::DisplayOn);
        Systick::DelayMilliseconds(15);

        WriteCommand(Command::SleepOut);
        Systick::DelayMilliseconds(15);
        
        _chipSelectPort->BSRR = (1 << _chipSelectPin);
    }

    void DrawFrame() const
    {
        _chipSelectPort->BSRR = (1 << (_chipSelectPin + 16));

        WriteCommand(Command::MemoryWrite);
        for(uint32_t i = 0; i < _dimensions.Width * _dimensions.Height; ++i)
        {
            DrawPointAtCurrentPosition((*_framebuffer)[i]);
        }
        while(!(_spi->SR & SPI_SR_TXE));
        while(_spi->SR & SPI_SR_BSY);
        
        _chipSelectPort->BSRR = (1 << _chipSelectPin);
    }

    Orientation GetOrientation() const
    {
        return _orientation;
    }

    Ili9341(Ili9341 const &) = delete;
    Ili9341 &operator=(Ili9341 const &) = delete;

private:

    Ili9341() = default;

    void Swap(Ili9341<width, height> &other)
    {
        std::swap(_framebuffer, other._framebuffer);
        std::swap(_spi, other._spi);
        std::swap(_chipSelectPort, other._chipSelectPort);
        std::swap(_dataCommandSelectPort, other._dataCommandSelectPort);
        std::swap(_connectionModeSelectPort, other._connectionModeSelectPort);
        std::swap(_chipSelectPin, other._chipSelectPin);
        std::swap(_dataCommandSelectPin, other._dataCommandSelectPin);
        std::swap(_connectionModeSelectPins, other._connectionModeSelectPins);
        _origin.Swap(other._origin);
        _dimensions.Swap(other._dimensions);
        std::swap(_orientation, other._orientation);
    }

    void Reset() const
    {
        WriteCommand(Command::Reset);
        Systick::DelayMilliseconds(1);
    }

    void SetColorMode() const
    {
        WriteCommand(Command::PixelFormatSet);
        WriteDataByte(0x55);
        Systick::DelayMilliseconds(1);
    }

    void SetOrientation(Orientation orientation)
    {
        WriteCommand(Command::MemoryAccessControl);
        if (orientation == Orientation::Portrait)
            WriteDataByte(0x48);
        else if (orientation == Orientation::Landscape)
            WriteDataByte(0x28);
        _orientation = orientation;
        Systick::DelayMilliseconds(1);
    }

    void SetSize(Point origin, Dimensions dimensions) const
    {
        WriteCommand(Command::ColumnAddressSet);
        uint8_t highByte = origin.X >> 8;
        uint8_t lowByte = origin.X;
        WriteDataByte(highByte);
        WriteDataByte(lowByte);
        highByte = (origin.X + dimensions.Width - 1) >> 8;
        lowByte = (origin.X + dimensions.Width - 1);
        WriteDataByte(highByte);
        WriteDataByte(lowByte);
        Systick::DelayMilliseconds(1);

        WriteCommand(Command::PageAddressSet);
        highByte = origin.Y >> 8;
        lowByte = origin.Y;
        WriteDataByte(highByte);
        WriteDataByte(lowByte);        
        highByte = (origin.Y + dimensions.Height - 1) >> 8;
        lowByte = (origin.Y + dimensions.Height - 1);
        WriteDataByte(highByte);
        WriteDataByte(lowByte);
        Systick::DelayMilliseconds(1);
    }

    void DrawPointAtCurrentPosition(Color565 color) const
    {
        DrawPointAtCurrentPosition(static_cast<uint16_t>(color));
    }

    void DrawPointAtCurrentPosition(uint16_t color) const
    {
        WriteDataWord(color);
    }

    void WriteCommand(Command command) const
    {
        WriteCommand(static_cast<uint8_t>(command));
    }

    void WriteCommand(uint8_t command) const
    {
        while(!(_spi->SR & SPI_SR_TXE));
        while(_spi->SR & SPI_SR_BSY);
        _dataCommandSelectPort->BSRR = (1 << (_dataCommandSelectPin + 16));
        
        _spi->DR = command;
        
        while(!(_spi->SR & SPI_SR_TXE));
        while(_spi->SR & SPI_SR_BSY);
        _dataCommandSelectPort->BSRR = (1 << _dataCommandSelectPin);
    }

    void WriteDataByte(uint8_t data) const
    {
        WriteData(data);
    }

    void WriteDataWord(uint16_t data) const
    {
        WriteData(data >> 8);
        WriteData(data);
    }

    void WriteData(uint8_t data) const
    {
        while(!(_spi->SR & SPI_SR_TXE));
        _spi->DR = data;
    }

    void SetConnectionMode() const
    {
        _connectionModeSelectPort->BSRR =
            (1 << _connectionModeSelectPins[1]) |
            (1 << _connectionModeSelectPins[2]);
        _connectionModeSelectPort->BSRR = 
            (1 << (_connectionModeSelectPins[0] + 16)) |
            (1 << (_connectionModeSelectPins[4] + 16));
    }

    Framebuffer<width, height> *_framebuffer;

    SPI_TypeDef *_spi;
    GPIO_TypeDef *_chipSelectPort;
    GPIO_TypeDef *_dataCommandSelectPort;
    GPIO_TypeDef *_connectionModeSelectPort;
    uint8_t _chipSelectPin;
    uint8_t _dataCommandSelectPin;
    uint8_t _connectionModeSelectPins[4];

    Point _origin;
    Dimensions _dimensions;
    Orientation _orientation;

    Point _cursorPosition;
};
