#pragma once

#include <cstdint>
#include <utility>

#include "stm32f4xx.h"

enum class PhysicalInterface
{
    Serial8Bit4Wire = 0x6
};

class Ili9341
{
public:

    Ili9341(Ili9341 && other)
    {
        Swap(other);
    }

    Ili9341 & operator=(Ili9341 && other)
    {
        Swap(other);
        return *this;
    }

    static Ili9341 ForSerial8Bit4Wire(SPI_TypeDef * spi,
        GPIO_TypeDef * chipSelectPort, uint8_t chipSelectPinNumber,
        GPIO_TypeDef * _dataCommandSelectPort, uint8_t _dataCommandSelectPinNumber,
        GPIO_TypeDef * spiPort, uint8_t spiClockPinNumber, uint8_t spiIoPinNumber,
        GPIO_TypeDef * vSyncPort, uint8_t vSyncPinNumber,
        GPIO_TypeDef * hSyncPort, uint8_t hSyncPinNumber)
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

    void Init()
    {
        _dataCommandSelectPort->MODER |= (1 << (_dataCommandSelectPinNumber << 1));
        _vSyncPort->MODER |= (1 << (_vSyncPinNumber << 1));
        _hSyncPort->MODER |= (1 << (_hSyncPinNumber << 1));
        _chipSelectPort->MODER |= (1 << (_chipSelectPinNumber << 1));
        _chipSelectPort->BSRR = (1 << _chipSelectPinNumber);

        _spiPort->MODER |= (2 << (_spiClockPinNumber << 1)) | (2 << (_spiIoPinNumber << 1));
        _spiPort->AFR[0] |= (5 << GPIO_AFRL_AFSEL7_Pos);
        _spiPort->AFR[1] |= (5 << GPIO_AFRH_AFSEL9_Pos);

        _spi->CR1 |= SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE | SPI_CR1_BR | SPI_CR1_MSTR | SPI_CR1_SPE;
    }

    void WriteCommand(uint8_t command)
    {
        _chipSelectPort->BSRR = (1 << (_chipSelectPinNumber << 16));
        _spi->CR1 |= SPI_CR1_BIDIOE;
        _dataCommandSelectPort->BSRR = (1 << (_dataCommandSelectPinNumber << 16));



        _chipSelectPort->BSRR = (1 << _chipSelectPinNumber);
    }

    void WriteData(uint8_t data)
    {
        _chipSelectPort->BSRR = (1 << (_chipSelectPinNumber << 16));
        _spi->CR1 |= SPI_CR1_BIDIOE;
        _dataCommandSelectPort->BSRR = (1 << _dataCommandSelectPinNumber);



        _chipSelectPort->BSRR = (1 << _chipSelectPinNumber);
    }

    uint8_t ReadData()
    {
        _chipSelectPort->BSRR = (1 << (_chipSelectPinNumber << 16));
        _spi->CR1 &= ~(SPI_CR1_BIDIOE);



        _chipSelectPort->BSRR = (1 << _chipSelectPinNumber);
        return 0;
    }

    PhysicalInterface GetPhysicalInterface() const
    {
        return _physicalInterface;
    }

    Ili9341(Ili9341 const &) = delete;
    Ili9341 & operator=(Ili9341 const &) = delete;

private:
    Ili9341() = default;

    void Swap(Ili9341 & other)
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
    
    SPI_TypeDef * _spi;

    GPIO_TypeDef * _chipSelectPort;
    GPIO_TypeDef * _dataCommandSelectPort;
    GPIO_TypeDef * _spiPort;
    GPIO_TypeDef * _vSyncPort;
    GPIO_TypeDef * _hSyncPort;

    uint8_t _chipSelectPinNumber;
    uint8_t _dataCommandSelectPinNumber;
    uint8_t _spiClockPinNumber;
    uint8_t _spiIoPinNumber;
    uint8_t _vSyncPinNumber;
    uint8_t _hSyncPinNumber;

    PhysicalInterface _physicalInterface = PhysicalInterface::Serial8Bit4Wire;
};
