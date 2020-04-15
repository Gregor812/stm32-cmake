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

class Ili9341
{
public:

    static Ili9341 ForSerial8Bit4Wire(SPI_TypeDef *spi,
        GPIO_TypeDef *chipSelectPort, uint8_t chipSelectPin,
        GPIO_TypeDef *dataCommandSelectPort, uint8_t dataCommandSelectPin,
        GPIO_TypeDef *connectionModeSelectPort,
        uint8_t (&connectionModeSelectPins)[4],
        Point origin, Dimensions dimensions,
        Orientation orientation)
    {
        Ili9341 result;
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
        result._cursorPosition = origin;
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

    void FillRectangle(Point origin, Dimensions dimensions, uint16_t color)
    {
        SetCursorPosition(origin);
        auto flattenedOrigin = _cursorPosition.X +
            _cursorPosition.Y * _dimensions.Width;
        for (uint16_t i = 0; i < dimensions.Height; ++i)
        {
            auto flattenedPosition = flattenedOrigin + 
                i * _dimensions.Width;
            for (uint16_t j = 0; j < dimensions.Width; ++j)
                _framebuffer[flattenedPosition++] = color;
        }
    }

    void FillRectangle(Point origin, Dimensions dimensions, Color565 color)
    {
        FillRectangle(origin, dimensions, static_cast<uint16_t>(color));
    }

    void FillBackground(uint16_t color)
    {
        FillRectangle(_origin, _dimensions, color);
    }

    void FillBackground(Color565 color)
    {
        FillBackground(static_cast<uint16_t>(color));
    }

    void DrawPoint(Color565 color, Point newPosition)
    {
        _chipSelectPort->BSRR = (1 << (_chipSelectPin + 16));

        SetCursorPosition(newPosition);
        WriteCommand(Command::MemoryWrite);
        DrawPointAtCurrentPosition(color);
        Systick::DelayMilliseconds(1);
        
        _chipSelectPort->BSRR = (1 << _chipSelectPin);
    }

    void DrawLine(uint16_t color, Point start, Point end)
    {
        bool inverted = false;

        int dx = end.X - start.X;
        int dy = end.Y - start.Y;

        if (std::abs(dx) < std::abs(dy))
        {
            std::swap(start.X, start.Y);
            std::swap(end.X, end.Y);
            dx = end.X - start.X;
            dy = end.Y - start.Y;
            inverted = true;
        }

        if (dx < 0)
        {
            std::swap(start, end);
            dx = -dx;
            dy = -dy;
        }

        int normalizedSlope = 0;
        int normalizedError = 0;
        
        if (dx != 0)
            normalizedSlope = std::abs(dy) * 2;

        uint16_t y = start.Y;

        for (uint16_t x = start.X; x <= end.X; ++x)
        {
            auto flattenedPosition = (inverted == true) ?
                x * _dimensions.Width + y :
                y * _dimensions.Width + x;
            _framebuffer[flattenedPosition] = color;

            normalizedError += normalizedSlope;
            if (normalizedError > dx)
            {
                normalizedError -= dx * 2;
                y += ((dy > 0) ? 1 : -1);
            }
        }
    }
    
    void DrawLine(Color565 color, Point start, Point end)
    {
        DrawLine(static_cast<uint16_t>(color), start, end);
    }

    void DrawSymbol(uint8_t *symbolMap,
        Point origin, Dimensions dimensions,
        uint16_t backgroundColor, uint16_t foregroundColor)
    {
        SetCursorPosition(origin);
        auto flattenedOrigin = _cursorPosition.Y * _dimensions.Width +
            _cursorPosition.X;
        for (uint16_t i = 0; i < dimensions.Height; ++i)
        {
            auto flattenedPosition = flattenedOrigin + 
                i * _dimensions.Width;
            for (uint16_t j = 0; j < dimensions.Width; ++j)
                _framebuffer[flattenedPosition++] = 
                    (symbolMap[i * dimensions.Width + j]) ?
                        foregroundColor :
                        backgroundColor;
        }
    }    

    void DrawSymbol(uint8_t *symbolMap,
        Point origin, Dimensions dimensions,
        Color565 backgroundColor, Color565 foregroundColor)
    {
        DrawSymbol(symbolMap, origin, dimensions,
            static_cast<uint16_t>(backgroundColor),
            static_cast<uint16_t>(foregroundColor));
    }

    void DrawFrame() const
    {
        _chipSelectPort->BSRR = (1 << (_chipSelectPin + 16));

        WriteCommand(Command::MemoryWrite);
        for(uint32_t i = 0; i < _dimensions.Width * _dimensions.Height; ++i)
        {
            DrawPointAtCurrentPosition(_framebuffer[i]);
        }
        Systick::DelayMilliseconds(1);
        
        _chipSelectPort->BSRR = (1 << _chipSelectPin);
    }

    Orientation GetOrientation() const
    {
        return _orientation;
    }

    Ili9341(Ili9341 const &) = delete;
    Ili9341 &operator=(Ili9341 const &) = delete;

private:

    enum class WriteMode
    {
        Command = 0,
        Data = 1
    };

    Ili9341() = default;

    void Swap(Ili9341 &other)
    {
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
        _cursorPosition.Swap(other._cursorPosition);
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

    void SetCursorPosition(Point newPosition)
    {
        _cursorPosition = newPosition;
    }

    void DrawPointAtCurrentPosition(Color565 color) const
    {
        DrawPointAtCurrentPosition(static_cast<uint16_t>(color));
    }

    void DrawPointAtCurrentPosition(uint16_t color) const
    {
        uint16_t data = static_cast<uint16_t>(color);
        uint8_t highByte = data >> 8;
        uint8_t lowByte = data;
        WriteDataByte(highByte);
        WriteDataByte(lowByte);
    }

    void WriteCommand(uint8_t command) const
    {
        WriteSerial(command, WriteMode::Command);
    }

    void WriteCommand(Command command) const
    {
        WriteSerial(static_cast<uint8_t>(command), WriteMode::Command);
    }

    void WriteDataByte(uint8_t data) const
    {
        WriteSerial(data, WriteMode::Data);
    }

    void WriteDataWord(uint16_t data) const
    {
        WriteSerial(data >> 8, WriteMode::Data);
        WriteSerial(data, WriteMode::Data);
    }

    void WriteSerial(uint8_t data, WriteMode writeMode) const
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
            Systick::DelayMilliseconds(1);
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

    uint16_t _framebuffer[76800];

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
