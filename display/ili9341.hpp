#pragma once

#include <cstdint>
#include <utility>

#include "stm32f4xx.h"
#include "systick.hpp"

#include "ConnectionMode.hpp"
#include "ColorMode.hpp"
#include "Color565.hpp"
#include "Command.hpp"
#include "Point.hpp"

class Ili9341
{
public:

    static Ili9341 ForSerial8Bit4Wire(SPI_TypeDef *spi,
        GPIO_TypeDef *chipSelectPort, uint8_t chipSelectPin,
        GPIO_TypeDef *dataCommandSelectPort, uint8_t dataCommandSelectPin,
        GPIO_TypeDef *connectionModeSelectPort,
        uint8_t (&connectionModeSelectPins)[4],
        Point origin,
        uint16_t width, uint8_t height,
        ColorMode colorMode)
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
        result._width = width;
        result._height = height;
        result._colorMode = colorMode;

        result.SetConnectionMode(ConnectionMode::Serial8Bit4Wire);
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

    void Init() const
    {
        _chipSelectPort->BSRR = (1 << (_chipSelectPin + 16));

        WriteCommand(Command::Reset);
        Systick::DelayMilliseconds(15);
        
        WriteCommand(Command::ColumnAddressSet);
        uint8_t highByte = _origin.X >> 8;
        uint8_t lowByte = _origin.X;
        WriteData(highByte);
        WriteData(lowByte);
        highByte = (_origin.X + _width - 1) >> 8;
        lowByte = (_origin.X + _width - 1);
        WriteData(highByte);
        WriteData(lowByte);
        Systick::DelayMilliseconds(15);

        WriteCommand(Command::PageAddressSet);
        highByte = _origin.Y >> 8;
        lowByte = _origin.Y;
        WriteData(highByte);
        WriteData(lowByte);
        highByte = (_origin.Y + _height - 1) >> 8;
        lowByte = (_origin.Y + _height - 1);
        WriteData(highByte);
        WriteData(lowByte);
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

    void FillBackground(Color565 color) const
    {
        _chipSelectPort->BSRR = (1 << (_chipSelectPin + 16));
        WriteCommand(Command::MemoryWrite);
        for(uint32_t i = 0; i < _width * _height; ++i)
        {
            DrawPoint(color);
        }

        Systick::DelayMilliseconds(1);
        _chipSelectPort->BSRR = (1 << _chipSelectPin);
    }

    void DrawPoint(Color565 color) const
    {
        if (_colorMode != ColorMode::R5G6B5)
            return;

        uint16_t data = static_cast<uint16_t>(color);
        uint8_t highByte = data >> 8;
        uint8_t lowByte = data;
        WriteData(highByte);
        WriteData(lowByte);
    }

    ConnectionMode GetPhysicalInterface() const
    {
        return _physicalInterface;
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
        _origin.Swap(other._origin);
        std::swap(_chipSelectPin, other._chipSelectPin);
        std::swap(_dataCommandSelectPin, other._dataCommandSelectPin);        
        std::swap(_connectionModeSelectPins, other._connectionModeSelectPins);
        std::swap(_width, other._width);
        std::swap(_height, other._height);
        std::swap(_colorMode, other._colorMode);
        std::swap(_physicalInterface, other._physicalInterface);
    }

    void WriteCommand(uint8_t command) const
    {
        Write(command, WriteMode::Command);
    }

    void WriteCommand(Command command) const
    {
        Write(static_cast<uint8_t>(command), WriteMode::Command);
    }

    void WriteData(uint8_t data) const
    {
        Write(data, WriteMode::Data);
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
            Systick::DelayMilliseconds(1);
    }

    void SetConnectionMode(ConnectionMode connectionMode)
    {
        switch (connectionMode)
        {
        case ConnectionMode::Serial8Bit4Wire:
        default:
            _connectionModeSelectPort->BSRR = (1 << _connectionModeSelectPins[1]) |
                (1 << _connectionModeSelectPins[2]);
            _connectionModeSelectPort->BSRR = 
                (1 << (_connectionModeSelectPins[0] + 16)) |
                (1 << (_connectionModeSelectPins[4] + 16));
            _physicalInterface = ConnectionMode::Serial8Bit4Wire;
            break;
        }
    }

    SPI_TypeDef *_spi;
    GPIO_TypeDef *_chipSelectPort;
    GPIO_TypeDef *_dataCommandSelectPort;
    GPIO_TypeDef *_connectionModeSelectPort;
    Point _origin;
    uint8_t _chipSelectPin;
    uint8_t _dataCommandSelectPin;    
    uint8_t _connectionModeSelectPins[4];
    uint16_t _width;
    uint8_t _height;

    ColorMode _colorMode;

    ConnectionMode _physicalInterface = ConnectionMode::Serial8Bit4Wire;
};
