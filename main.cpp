#include "stm32f4xx.h"

#include "config.h"
#include "ili9341.hpp"

void SystemClock_Config(void);
void Timer2_Config(void);

unsigned volatile state = 0;

int main(void)
{
    SystemClock_Config();

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN |
        RCC_AHB1ENR_GPIOCEN |
        RCC_AHB1ENR_GPIODEN |
        RCC_AHB1ENR_GPIOFEN |
        RCC_AHB1ENR_GPIOGEN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
    GPIOG->MODER |= (1 << GPIO_MODER_MODER13_Pos) | (1 << GPIO_MODER_MODER14_Pos);
    
    Timer2_Config();
    Ili9341 display = Ili9341::ForSerial8Bit4Wire(SPI5,
        GPIOC, 2,
        GPIOD, 13,
        GPIOF, 7, 9,
        GPIOA, 4,
        GPIOC, 6);

    display.Init();

    display.DrawColor(0x00);

    while (1)
    {
    }
}

void SystemClock_Config(void)
{
    RCC->CR |= RCC_CR_HSEON;

    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM_Msk | RCC_PLLCFGR_PLLN_Msk | RCC_PLLCFGR_PLLQ_Msk);
    RCC->PLLCFGR |= (RCC_PLLCFGR_PLLSRC_HSE | (4 << RCC_PLLCFGR_PLLM_Pos) | (168 << RCC_PLLCFGR_PLLN_Pos) | (7 << RCC_PLLCFGR_PLLQ_Pos));
    FLASH->ACR |= 5;

    while ((RCC->CR & RCC_CR_HSERDY) == 0)
    {
    }

    RCC->CR |= RCC_CR_PLLON;
    RCC->CFGR |= 4 << RCC_CFGR_PPRE2_Pos;
    RCC->CFGR |= 5 << RCC_CFGR_PPRE1_Pos;

    while ((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    RCC->CFGR |= RCC_CFGR_SW_PLL;

    while ((RCC->CFGR & RCC_CFGR_SWS_PLL) == 0)
    {
    }

    RCC->CR &= ~(RCC_CR_HSION);
}

void Timer2_Config(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 21000 - 1;
    TIM2->ARR = 4000 - 1;
    TIM2->EGR |= TIM_EGR_UG;
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_SetPriority(TIM2_IRQn, 0);
    NVIC_EnableIRQ(TIM2_IRQn);
    TIM2->CR1 |= TIM_CR1_CEN;
}

extern "C" void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~(TIM_SR_UIF);

    switch (state)
    {
    case 0:
        GPIOG->BSRR = 1 << 13;
        break;
    case 1:
        GPIOG->BSRR = 1 << 14;
        break;
    case 2:
        GPIOG->BSRR = 1 << (13 + 16);
        break;
    case 3:
        GPIOG->BSRR = 1 << (14 + 16);
        break;
    }

    if (++state > 3)
        state = 0;
}
