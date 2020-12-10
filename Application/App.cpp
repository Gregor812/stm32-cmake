#include "App.hpp"
#include "Systick.hpp"
#include "Random.hpp"
#include "SpiDriver.hpp"

using namespace Peripherals;

namespace Application
{
    void App::Run()
    {
        ConfigureSystemClock();
        InitializeDevice();

        while (true);
    }

    void App::ConfigureSystemClock(void)
    {
        RCC->CR |= RCC_CR_HSEON;

        RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM_Msk | RCC_PLLCFGR_PLLN_Msk |
                        RCC_PLLCFGR_PLLQ_Msk);
        RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE | (4 << RCC_PLLCFGR_PLLM_Pos) |
                        (168 << RCC_PLLCFGR_PLLN_Pos) | (7 << RCC_PLLCFGR_PLLQ_Pos);
        FLASH->ACR |= 5;

        while ((RCC->CR & RCC_CR_HSERDY) == 0);

        RCC->CR |= RCC_CR_PLLON;
        RCC->CFGR |= 5 << RCC_CFGR_PPRE2_Pos;
        RCC->CFGR |= 5 << RCC_CFGR_PPRE1_Pos;

        while ((RCC->CR & RCC_CR_PLLRDY) == 0);

        RCC->CFGR |= RCC_CFGR_SW_PLL;
        while ((RCC->CFGR & RCC_CFGR_SWS_PLL) == 0);

        RCC->CR &= ~(RCC_CR_HSION);
        Systick::Initialize();
    }

    void App::InitializeDevice(void)
    {
        Random::Initialize();
        InitializePins();
        auto ili9341 = Ili9341::SpiDriver(SPI5, [this](SPI_TypeDef *spi) { this->InitializeSpi(spi); });
        ili9341.InitializeHardware();
        ili9341.InitializeSoftware();
        ili9341.RedrawFrame();
    }

    void App::InitializePins(void)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN |
                        RCC_AHB1ENR_GPIOFEN;

        Gpio::SetMode<Gpio::Pin::Pin2, Gpio::Pin::Pin4,
            Gpio::Pin::Pin5, Gpio::Pin::Pin7, Gpio::Pin::Pin13>
            (GPIOD, Gpio::Mode::Output);

        Gpio::SetSpeed<Gpio::Pin::Pin13>(GPIOD, Gpio::Speed::VeryHigh);

        Gpio::Set<Gpio::Pin::Pin2>(GPIOC);
        Gpio::SetMode<Gpio::Pin::Pin2>(GPIOC, Gpio::Mode::Output);
        Gpio::SetSpeed<Gpio::Pin::Pin2>(GPIOC, Gpio::Speed::VeryHigh);

        Gpio::SetMode<Gpio::Pin::Pin7, Gpio::Pin::Pin9>(GPIOF, Gpio::Mode::OutputAF);
        Gpio::SetSpeed<Gpio::Pin::Pin7, Gpio::Pin::Pin9>(GPIOF, Gpio::Speed::VeryHigh);
        Gpio::SetAltFunction<Gpio::Pin::Pin7, Gpio::Pin::Pin9>(GPIOF, Gpio::AltFunction::AF5);
    }

    void App::InitializeSpi(SPI_TypeDef *spi)
    {
        RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
        spi->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE;
    }
}
