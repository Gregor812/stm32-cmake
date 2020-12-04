#pragma once

#include <Ioutput.hpp>
#include "stm32f4xx.h"

class Spi : public IOutput
{
public:

    Spi(SPI_TypeDef *spi, GPIO_TypeDef *chipSelectPort,
        uint8_t chipSelectPin) :
        _spi(spi), _chipSelectPort(chipSelectPort),
        _chipSelectPin(chipSelectPin)
    {
        ActivateChipSelect();
    }

    void Write(uint8_t *bufferStart, uint8_t *bufferEnd) override
    {
        
        for (;bufferStart != bufferEnd; ++bufferStart)
            WriteByte(*bufferStart);
        
        ReleaseChipSelect();
    }

    bool IsBusy() override
    {
        return !(_spi->SR & SPI_SR_TXE) || (_spi->SR & SPI_SR_BSY);
    }

    ~Spi()
    {
        ReleaseChipSelect();
    }

private:
    void WriteByte(uint8_t byte)
    {
        while(!(_spi->SR & SPI_SR_TXE));
        _spi->DR = byte;
    }

    void ActivateChipSelect()
    {
        _chipSelectPort->BSRR = (1 << (_chipSelectPin + 16));
    }

    void ReleaseChipSelect()
    {
         _chipSelectPort->BSRR = (1 << _chipSelectPin);
    }

    SPI_TypeDef *_spi;
    GPIO_TypeDef *_chipSelectPort;
    uint8_t _chipSelectPin;
};
