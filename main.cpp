#include "stm32f4xx.h"
#include "config.h"
#include "Ili9341.hpp"
#include "Random.hpp"

void SystemClock_Config(void);
void ApplicationInit(void);
void Draw();

const uint32_t DmaSendingSize = 320 * 240 / 2;

static volatile uint32_t counter;
static SPI_TypeDef *displaySpi = SPI5;
static GPIO_TypeDef *chipSelectPort = GPIOC;
static uint8_t chipSelectPin = 2;
static GPIO_TypeDef *dataCommandSelectPort = GPIOD;
static uint8_t dataCommandSelectPin = 13;
static GPIO_TypeDef *spiPort = GPIOF;
static GPIO_TypeDef *connectionModeSelectPort = GPIOD;
static uint8_t connectionModePins[4] = {2, 4, 5, 7};
static Point origin = {0, 0};
static Dimensions dimensions = {320, 240};
static Framebuffer<320, 240> framebuffer(origin);

static Ili9341<320, 240> display = Ili9341<320, 240>::ForSerial8Bit4Wire(
    &framebuffer, displaySpi,
    chipSelectPort, chipSelectPin,
    dataCommandSelectPort, dataCommandSelectPin,
    connectionModeSelectPort, connectionModePins,
    origin, dimensions, Orientation::Landscape);

int main(void)
{
    SystemClock_Config();
    ApplicationInit();

    display.Init();
    
    framebuffer.FillBackground(0x00F8);
    display.StartFrameDrawing();
    while (true) Draw();
}

void SystemClock_Config(void)
{
    RCC->CR |= RCC_CR_HSEON;

    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM_Msk | RCC_PLLCFGR_PLLN_Msk |
        RCC_PLLCFGR_PLLQ_Msk);
    RCC->PLLCFGR |= (RCC_PLLCFGR_PLLSRC_HSE | (4 << RCC_PLLCFGR_PLLM_Pos) |
        (168 << RCC_PLLCFGR_PLLN_Pos) | (7 << RCC_PLLCFGR_PLLQ_Pos));
    FLASH->ACR |= 5;

    while ((RCC->CR & RCC_CR_HSERDY) == 0)
    {}

    RCC->CR |= RCC_CR_PLLON;
    RCC->CFGR |= 5 << RCC_CFGR_PPRE2_Pos;
    RCC->CFGR |= 5 << RCC_CFGR_PPRE1_Pos;

    while ((RCC->CR & RCC_CR_PLLRDY) == 0)
    {}

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS_PLL) == 0)
    {}

    RCC->CR &= ~(RCC_CR_HSION);
    Systick::InitSystick();
}

void ApplicationInit(void)
{
    counter = 0;

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN |
        RCC_AHB1ENR_GPIOFEN | RCC_AHB1ENR_GPIOGEN;

    for (size_t i = 0; i < sizeof(connectionModePins); ++i)
    {
        connectionModeSelectPort->MODER |= (1 << (connectionModePins[i] << 1));
    }
    
    dataCommandSelectPort->MODER |= (1 << (dataCommandSelectPin << 1));
    dataCommandSelectPort->OSPEEDR |= (3 << (dataCommandSelectPin << 1));

    chipSelectPort->BSRR = (1 << chipSelectPin);
    chipSelectPort->MODER |= (1 << (chipSelectPin << 1));
    chipSelectPort->OSPEEDR |= (3 << (chipSelectPin << 1));

    spiPort->MODER |= (2 << GPIO_MODER_MODER7_Pos) | (2 << GPIO_MODER_MODER9_Pos);
    spiPort->OSPEEDR |= (3 << GPIO_OSPEEDR_OSPEED7_Pos) |
        (3 << GPIO_OSPEEDR_OSPEED9_Pos);
    spiPort->AFR[0] |= (5 << GPIO_AFRL_AFSEL7_Pos);
    spiPort->AFR[1] |= (5 << GPIO_AFRH_AFSEL9_Pos);

    RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
    displaySpi->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE;
    displaySpi->CR2 |= SPI_CR2_TXDMAEN;

    RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
    RNG->CR |= RNG_CR_RNGEN;
}

void Draw()
{
    uint16_t shift = 320 - 20 - 1;
    do
    {
        auto color = Random::GetUint32(0, 0x10000);
        framebuffer.DrawLine(color, {shift, 240 - 20 - 1}, {shift, 240 - 90 - 1});
    }
    while (--shift > 320 - 90 - 1);
}

extern "C" void DMA2_Stream6_IRQHandler()
{
    if (DMA2->HISR & DMA_HISR_TCIF6)
    {
        DMA2->HIFCR |= DMA_HIFCR_CTCIF6;

        if (counter++ < 3);
        else counter = 0;

        DMA2_Stream6->M0AR = reinterpret_cast<uint32_t>(framebuffer.GetRawPointer()) +
            counter * DmaSendingSize;
        DMA2_Stream6->CR |= DMA_SxCR_EN;
    }
}
