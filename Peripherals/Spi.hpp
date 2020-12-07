#pragma once

#include "stm32f4xx.h"
#include <cstdint>
#include <type_traits>
#include <functional>

namespace Peripherals
{
    enum class DataLength : uint8_t
    {
        OneByte,
        TwoBytes
    };


    template <DataLength dataLength,
        typename TData,
        typename = std::enable_if_t<
            (dataLength == DataLength::OneByte && std::is_same<TData, uint8_t>::value) ||
            (dataLength == DataLength::TwoBytes && std::is_same<TData, uint16_t>::value)>>
    class Spi
    {
    public:

        Spi() = default;

        Spi(SPI_TypeDef *spi, const std::function<void (SPI_TypeDef *)>& initialize) : 
            _spi(spi), _initialize(initialize)
        {}

        Spi& Initialize()
        {
            _initialize(_spi);
            return *this;
        }

        Spi& ActivateChipSelect()
        {
            GPIOC->BSRR = 1 << (2 + 16);
            return *this;
        }

        Spi& ReleaseChipSelect()
        {
            GPIOC->BSRR = 1 << 2;
            return *this;
        }

        Spi& Write(TData data)
        {
            while (!BufferIsEmpty());
            _spi->DR = data;
            return *this;
        }

        bool IsBusy() const
        {
            return _spi->SR & SPI_SR_BSY;
        }

        bool BufferIsEmpty() const
        {
            return _spi->SR & SPI_SR_TXE;
        }

    private:

        SPI_TypeDef *_spi;
        std::function<void (SPI_TypeDef *)> _initialize;
    };

    using Spi8Bit = Spi<DataLength::OneByte, uint8_t>;
    using Spi16Bit = Spi<DataLength::TwoBytes, uint16_t>;
}
