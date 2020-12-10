#pragma once

#include <cstdint>
#include <type_traits>
#include <functional>

#include "stm32f4xx.h"
#include "Gpio.hpp"

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

        Spi(SPI_TypeDef *spi, const std::function<void (SPI_TypeDef *)>& initialize) : 
            _spi(spi), _initialize(initialize)
        {}

        void Initialize(void)
        {
            _initialize(_spi);
        }

        void ActivateChipSelect(void)
        {
            Gpio::Reset<Gpio::Pin::Pin2>(GPIOC);
        }

        void ReleaseChipSelect(void)
        {
            Gpio::Set<Gpio::Pin::Pin2>(GPIOC);
        }

        void Write(TData data)
        {
            while (!BufferIsEmpty());
            _spi->DR = data;
        }

        bool IsBusy(void) const
        {
            return _spi->SR & SPI_SR_BSY;
        }

        bool BufferIsEmpty(void) const
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
