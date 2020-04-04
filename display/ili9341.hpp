#pragma once

#include <cstdint>
#include <utility>

#include "stm32f4xx.h"

enum class PhysicalInterface
{
    Serial8Bit4Wire = 0x6
};

enum class Command : uint8_t
{
    Reset = 0x01,
    SleepOut = 0x11,
    GammaSet = 0x26,
    DisplayOff = 0x28,
    DisplayOn = 0x29,
    ColumnAddressSet = 0x2A,
    PageAddressSet = 0x2B,
    MemoryWrite = 0x2C,
    MemoryAccessControl = 0x36,
    PixelFormatSet = 0x3A,
    WriteDisplayBrightness = 0x51,
    WriteCTRLDisplay = 0x53,
    RgbInterfaceSignalControl = 0xB0,
    FrameControlNormalMode = 0xB1,
    BlankingPorchControl = 0xB5,
    DisplayFunctionControl = 0xB6,
    PowerControl1 = 0xC0,
    PowerControl2 = 0xC1,
    VComControl1 = 0xC5,
    VComControl2 = 0xC7,
    PowerControlA = 0xCB,
    PowerControlB = 0xCF,
    PositiveGammaCorrection = 0xE0,
    NegativeGammaCorrection = 0xE1,
    DriverTimingControlA = 0xE8,
    DriverTimingControlB = 0xEA,
    PowerOnSequenceControl = 0xED,
    Enable3GammaControl = 0xF2,
    InterfaceControl = 0xF6,
    PumpRatioControl = 0xF7
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
                                      GPIO_TypeDef *chipSelectPort, uint8_t chipSelectPinNumber,
                                      GPIO_TypeDef *_dataCommandSelectPort, uint8_t _dataCommandSelectPinNumber,
                                      GPIO_TypeDef *spiPort, uint8_t spiClockPinNumber, uint8_t spiIoPinNumber,
                                      GPIO_TypeDef *vSyncPort, uint8_t vSyncPinNumber,
                                      GPIO_TypeDef *hSyncPort, uint8_t hSyncPinNumber)
    {
        Ili9341 result;
        result._spi = spi;
        result._chipSelectPort = chipSelectPort;
        result._dataCommandSelectPort = _dataCommandSelectPort;
        result._spiPort = spiPort;
        result._vSyncPort = vSyncPort;
        result._hSyncPort = hSyncPort;
        result._chipSelectPinNumber = chipSelectPinNumber;
        result._dataCommandSelectPinNumber = _dataCommandSelectPinNumber;
        result._spiClockPinNumber = spiClockPinNumber;
        result._spiIoPinNumber = spiIoPinNumber;
        result._vSyncPinNumber = vSyncPinNumber;
        result._hSyncPinNumber = hSyncPinNumber;
        result._physicalInterface = PhysicalInterface::Serial8Bit4Wire;
        return result;
    }

    void Init() const
    {
        _dataCommandSelectPort->MODER |= (1 << (_dataCommandSelectPinNumber << 1));
        // _vSyncPort->MODER |= (1 << (_vSyncPinNumber << 1));
        // _hSyncPort->MODER |= (1 << (_hSyncPinNumber << 1));
        _chipSelectPort->BSRR = (1 << _chipSelectPinNumber);
        _chipSelectPort->MODER |= (1 << (_chipSelectPinNumber << 1));

        _spiPort->MODER |= (2 << (_spiClockPinNumber << 1)) | (2 << (_spiIoPinNumber << 1));
        _spiPort->OTYPER |= (1 << _spiClockPinNumber) | (1 << _spiIoPinNumber);
        _spiPort->PUPDR |= (1 << (_spiClockPinNumber << 1)) | (1 << (_spiIoPinNumber << 1));
        _spiPort->OSPEEDR |= (3 << (_spiClockPinNumber << 1)) | (3 << (_spiIoPinNumber << 1));
        _spiPort->AFR[0] |= (5 << GPIO_AFRL_AFSEL7_Pos);
        _spiPort->AFR[1] |= (5 << GPIO_AFRH_AFSEL9_Pos);

        _spi->CR1 |= (SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE | (7 << SPI_CR1_BR_Pos) | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI);
        _spi->CR1 |= SPI_CR1_SPE;

        while ((_spi->CR1 & SPI_CR1_SPE) == 0)
        {
        }

        WriteCommand(Command::PowerControlA);
        WriteData(0x39);
        WriteData(0x2C);
        WriteData(0x00);
        WriteData(0x34);
        WriteData(0x02);
        WriteCommand(Command::PowerControlB);
        WriteData(0x00);
        WriteData(0xC1);
        WriteData(0x30);
        WriteCommand(Command::DriverTimingControlA);
        WriteData(0x85);
        WriteData(0x00);
        WriteData(0x78);
        WriteCommand(Command::DriverTimingControlB);
        WriteData(0x00);
        WriteData(0x00);
        WriteCommand(Command::PowerOnSequenceControl);
        WriteData(0x64);
        WriteData(0x03);
        WriteData(0x12);
        WriteData(0x81);
        WriteCommand(Command::PumpRatioControl);
        WriteData(0x20);
        WriteCommand(Command::PowerControl1);
        WriteData(0x23);
        WriteCommand(Command::PowerControl2);
        WriteData(0x10);
        WriteCommand(Command::VComControl1);
        WriteData(0x3E);
        WriteData(0x28);
        WriteCommand(Command::VComControl2);
        WriteData(0x86);
        WriteCommand(Command::MemoryAccessControl);
        WriteData(0x48);
        WriteCommand(Command::PixelFormatSet);
        WriteData(0x55);
        WriteCommand(Command::FrameControlNormalMode);
        WriteData(0x00);
        WriteData(0x18);
        WriteCommand(Command::DisplayFunctionControl);
        WriteData(0x08);
        WriteData(0x82);
        WriteData(0x27);
        WriteCommand(Command::Enable3GammaControl);
        WriteData(0x00);
        WriteCommand(Command::ColumnAddressSet);
        WriteData(0x00);
        WriteData(0x00);
        WriteData(0x00);
        WriteData(0xEF);
        WriteCommand(Command::PageAddressSet);
        WriteData(0x00);
        WriteData(0x00);
        WriteData(0x01);
        WriteData(0x3F);
        WriteCommand(Command::GammaSet);
        WriteData(0x01);
        WriteCommand(Command::PositiveGammaCorrection);
        WriteData(0x0F);
        WriteData(0x31);
        WriteData(0x2B);
        WriteData(0x0C);
        WriteData(0x0E);
        WriteData(0x08);
        WriteData(0x4E);
        WriteData(0xF1);
        WriteData(0x37);
        WriteData(0x07);
        WriteData(0x10);
        WriteData(0x03);
        WriteData(0x0E);
        WriteData(0x09);
        WriteData(0x00);
        WriteCommand(Command::NegativeGammaCorrection);
        WriteData(0x00);
        WriteData(0x0E);
        WriteData(0x14);
        WriteData(0x03);
        WriteData(0x11);
        WriteData(0x07);
        WriteData(0x31);
        WriteData(0xC1);
        WriteData(0x48);
        WriteData(0x08);
        WriteData(0x0F);
        WriteData(0x0C);
        WriteData(0x31);
        WriteData(0x36);
        WriteData(0x0F);
        WriteCommand(Command::SleepOut);

        volatile uint16_t pause = 0xFFFF;
        while (pause--)
        {
        }

        WriteCommand(Command::DisplayOn);
        WriteCommand(Command::MemoryWrite);
    }

    void SetCursorPosition(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
    {
        WriteCommand(Command::ColumnAddressSet);
        WriteData(x1 >> 8);
        WriteData(x1 & 0xFF);
        WriteData(x2 >> 8);
        WriteData(x2 & 0xFF);
    
        WriteCommand(Command::PageAddressSet);
        WriteData(y1 >> 8);
        WriteData(y1 & 0xFF);
        WriteData(y2 >> 8);
        WriteData(y2 & 0xFF);
    }

    void DrawPixel(uint16_t x, uint16_t y, uint16_t color)
    {
        SetCursorPosition(x, y, x, y);
        WriteCommand(Command::MemoryAccessControl);
        WriteData(color >> 8);
        WriteData(color & 0xFF);
    }

    void DrawColor(uint8_t color)
    {
        SetCursorPosition(0, 0, 240 - 1, 320 - 1);    
        WriteCommand(Command::MemoryAccessControl);
    
        for (uint32_t n = 0; n < 240 * 320; n++) 
        {
            WriteData(color);
            WriteData(color);
        }
    }


    void WriteCommand(Command command) const
    {
        Write(static_cast<uint8_t>(command), WriteMode::Command);
    }

    void WriteData(uint8_t data) const
    {
        Write(data, WriteMode::Data);
    }

    uint8_t ReadData() const
    {
        _chipSelectPort->BSRR = (1 << (_chipSelectPinNumber + 16));
        _spi->CR1 &= ~(SPI_CR1_BIDIOE);

        while ((_spi->SR & SPI_SR_RXNE) == 0)
        {
        }
        uint8_t data = _spi->DR;
        while (_spi->SR & SPI_SR_BSY)
        {
        }

        _chipSelectPort->BSRR = (1 << _chipSelectPinNumber);
        return data;
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
        std::swap(_vSyncPort, other._vSyncPort);
        std::swap(_hSyncPort, other._hSyncPort);

        std::swap(_chipSelectPinNumber, other._chipSelectPinNumber);
        std::swap(_dataCommandSelectPinNumber, other._dataCommandSelectPinNumber);
        std::swap(_spiClockPinNumber, other._spiClockPinNumber);
        std::swap(_spiIoPinNumber, other._spiIoPinNumber);
        std::swap(_vSyncPinNumber, other._vSyncPinNumber);
        std::swap(_hSyncPinNumber, other._hSyncPinNumber);

        std::swap(_physicalInterface, other._physicalInterface);
    }

    void Write(uint8_t data, WriteMode writeMode) const
    {
        _chipSelectPort->BSRR = (1 << (_chipSelectPinNumber + 16));
        _spi->CR1 |= SPI_CR1_BIDIOE;

        if (writeMode == WriteMode::Command)
            _dataCommandSelectPort->BSRR = (1 << (_dataCommandSelectPinNumber + 16));
        else
            _dataCommandSelectPort->BSRR = (1 << _dataCommandSelectPinNumber);

        while ((_spi->SR & SPI_SR_TXE) == 0)
        {
        }
        _spi->DR = data;
        while (_spi->SR & SPI_SR_TXE)
        {
        }

        _chipSelectPort->BSRR = (1 << _chipSelectPinNumber);
    }

    SPI_TypeDef *_spi;

    GPIO_TypeDef *_chipSelectPort;
    GPIO_TypeDef *_dataCommandSelectPort;
    GPIO_TypeDef *_spiPort;
    GPIO_TypeDef *_vSyncPort;
    GPIO_TypeDef *_hSyncPort;

    uint8_t _chipSelectPinNumber;
    uint8_t _dataCommandSelectPinNumber;
    uint8_t _spiClockPinNumber;
    uint8_t _spiIoPinNumber;
    uint8_t _vSyncPinNumber;
    uint8_t _hSyncPinNumber;

    PhysicalInterface _physicalInterface = PhysicalInterface::Serial8Bit4Wire;
};
