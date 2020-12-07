#include "App.hpp"
#include "Systick.hpp"
#include "Random.hpp"
#include "SpiDriver.hpp"

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
        auto ili9341 = Ili9341::SpiDriver(SPI5, [this](SPI_TypeDef *spi) { this->InitializeSpi(spi); })
            .InitializeHardware()
            .InitializeSoftware();
        ili9341.RedrawFrame();
    }

    void App::InitializePins(void)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN |
                        RCC_AHB1ENR_GPIOFEN;

        GPIOD->MODER |= (1 << (2 << 1)) | (1 << (4 << 1)) |
                        (1 << (5 << 1)) | (1 << (7 << 1));

        GPIOD->MODER |= (1 << (13 << 1));
        GPIOD->OSPEEDR |= (3 << (13 << 1));

        GPIOC->BSRR = (1 << 2);
        GPIOC->MODER |= (1 << (2 << 1));
        GPIOC->OSPEEDR |= (3 << (2 << 1));

        GPIOF->MODER |= (2 << GPIO_MODER_MODER7_Pos) | (2 << GPIO_MODER_MODER9_Pos);
        GPIOF->OSPEEDR |= (3 << GPIO_OSPEEDR_OSPEED7_Pos) |
                          (3 << GPIO_OSPEEDR_OSPEED9_Pos);
        GPIOF->AFR[0] |= (5 << GPIO_AFRL_AFSEL7_Pos);
        GPIOF->AFR[1] |= (5 << GPIO_AFRH_AFSEL9_Pos);
    }

    void App::InitializeSpi(SPI_TypeDef *spi)
    {
        RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
        spi->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE;
    }
}
