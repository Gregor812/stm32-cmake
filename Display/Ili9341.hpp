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
#include "Ioutput.hpp"

template<size_t width, size_t height>
class Ili9341
{
public:

    static Ili9341<width, height> ForSerial8Bit4Wire(
        Framebuffer<width, height> *framebuffer, IOutput *output,
        GPIO_TypeDef *dataCommandSelectPort,
        uint8_t dataCommandSelectPin,
        GPIO_TypeDef *connectionModeSelectPort,
        uint8_t (&connectionModeSelectPins)[4],
        Point origin, Dimensions dimensions,
        Orientation orientation)
    {
        Ili9341<width, height> result;
        result._framebuffer = framebuffer;
        result._output = output;
        result._dataCommandSelectPort = dataCommandSelectPort;
        result._connectionModeSelectPort = connectionModeSelectPort;
        result._dataCommandSelectPin = dataCommandSelectPin;
        result._connectionModeSelectPins[0] = connectionModeSelectPins[0];
        result._connectionModeSelectPins[1] = connectionModeSelectPins[1];        
        result._connectionModeSelectPins[2] = connectionModeSelectPins[2];
        result._connectionModeSelectPins[3] = connectionModeSelectPins[3];
        result._origin = origin;
        result._dimensions = dimensions;
        result._orientation = orientation;
        result._isDrawing = false;
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
        SetConnectionMode();
        Reset();
        SetColorMode();
        SetOrientation(_orientation);
        SetSize(_origin, _dimensions);

        WriteCommand(Command::DisplayOn);
        Systick::DelayMilliseconds(15);

        WriteCommand(Command::SleepOut);
        Systick::DelayMilliseconds(15);
    }

    void StartFrameDrawing()
    {
        if (_isDrawing) return;

        _isDrawing = true;
        WriteCommand(Command::MemoryWrite);

        NVIC_EnableIRQ(DMA2_Stream6_IRQn);
        
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
        DMA2->HIFCR &= ~DMA_HIFCR_CTCIF6;
        DMA2_Stream6->M0AR = reinterpret_cast<uint32_t>(_framebuffer->GetRawBuffer());
        DMA2_Stream6->PAR = reinterpret_cast<uint32_t>(&SPI5->DR);
        DMA2_Stream6->NDTR = width * height / 2;
        DMA2_Stream6->CR |= (1 << DMA_SxCR_DIR_Pos) | (3 << DMA_SxCR_PL_Pos) |
            (7 << DMA_SxCR_CHSEL_Pos) | DMA_SxCR_MINC | DMA_SxCR_TCIE;
        DMA2_Stream6->CR |= DMA_SxCR_EN;
    }

    void StopFrameDrawing()
    {
        if (!_isDrawing) return;

        while(_output->IsBusy());
        DMA2_Stream6->CR &= ~DMA_SxCR_EN;
        DMA2->HIFCR |= DMA_HIFCR_CTCIF6;
        RCC->AHB1ENR &= ~RCC_AHB1ENR_DMA2DEN;

        NVIC_DisableIRQ(DMA2_Stream6_IRQn);

        _isDrawing = false;
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
        std::swap(_output, other._output);
        std::swap(_dataCommandSelectPort, other._dataCommandSelectPort);
        std::swap(_connectionModeSelectPort, other._connectionModeSelectPort);
        std::swap(_dataCommandSelectPin, other._dataCommandSelectPin);
        std::swap(_connectionModeSelectPins, other._connectionModeSelectPins);
        _origin.Swap(other._origin);
        _dimensions.Swap(other._dimensions);
        std::swap(_orientation, other._orientation);
        std::swap(_isDrawing, other._isDrawing);
    }

    void Reset() const
    {
        WriteCommand(Command::Reset);
        Systick::DelayMilliseconds(1);
    }

    void SetColorMode() const
    {
        WriteCommand(Command::PixelFormatSet);
        WriteByte(0x55);
        Systick::DelayMilliseconds(1);
    }

    void SetOrientation(Orientation orientation)
    {
        WriteCommand(Command::MemoryAccessControl);
        if (orientation == Orientation::Portrait)
            WriteByte(0x48);
        else if (orientation == Orientation::Landscape)
            WriteByte(0x28);
        _orientation = orientation;
        Systick::DelayMilliseconds(1);
    }

    void SetSize(Point origin, Dimensions dimensions) const
    {
        WriteCommand(Command::ColumnAddressSet);
        uint8_t highByte = origin.X >> 8;
        uint8_t lowByte = origin.X;
        WriteByte(highByte);
        WriteByte(lowByte);
        highByte = (origin.X + dimensions.Width - 1) >> 8;
        lowByte = (origin.X + dimensions.Width - 1);
        WriteByte(highByte);
        WriteByte(lowByte);
        Systick::DelayMilliseconds(1);

        WriteCommand(Command::PageAddressSet);
        highByte = origin.Y >> 8;
        lowByte = origin.Y;
        WriteByte(highByte);
        WriteByte(lowByte);        
        highByte = (origin.Y + dimensions.Height - 1) >> 8;
        lowByte = (origin.Y + dimensions.Height - 1);
        WriteByte(highByte);
        WriteByte(lowByte);
        Systick::DelayMilliseconds(1);
    }

    void DrawPointAtCurrentPosition(Color565 color) const
    {
        DrawPointAtCurrentPosition(static_cast<uint16_t>(color));
    }

    void DrawPointAtCurrentPosition(uint16_t color) const
    {
        WriteHalfWord(color);
    }

    void WriteCommand(Command command) const
    {
        WriteCommand(static_cast<uint8_t>(command));
    }

    void WriteCommand(uint8_t command) const
    {
        while(_output->IsBusy());
        _dataCommandSelectPort->BSRR = (1 << (_dataCommandSelectPin + 16));

        WriteByte(command);

        while(_output->IsBusy());
        _dataCommandSelectPort->BSRR = (1 << _dataCommandSelectPin);
    }

    void WriteByte(uint8_t data) const
    {
        _output->Write(&data, (&data) + 1);
    }

    void WriteHalfWord(uint16_t data) const
    {
        WriteByte(data >> 8);
        WriteByte(data);
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

    IOutput *_output;
    GPIO_TypeDef *_dataCommandSelectPort;
    GPIO_TypeDef *_connectionModeSelectPort;
    uint8_t _dataCommandSelectPin;
    uint8_t _connectionModeSelectPins[4];

    bool _isDrawing;

    Point _origin;
    Dimensions _dimensions;
    Orientation _orientation;

    Point _cursorPosition;
};
