#include "stm32f4xx.h"
#include "config.h"
#include "Ili9341.hpp"

void SystemClock_Config(void);
void ApplicationInit(void);
void DrawOnDisplay(Ili9341 &display);

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
static uint8_t x[84] = {
    0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 1, 0,
    0, 1, 0, 0, 0, 1, 0,
    0, 0, 1, 0, 1, 0, 0,
    0, 0, 1, 0, 1, 0, 0,
    0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0,
    0, 0, 1, 0, 1, 0, 0,
    0, 0, 1, 0, 1, 0, 0,
    0, 1, 0, 0, 0, 1, 0,
    0, 1, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 0
};
static uint8_t y[84] = {
    0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 1, 0,
    0, 1, 0, 0, 0, 1, 0,
    0, 1, 0, 0, 0, 1, 0,
    0, 0, 1, 0, 1, 1, 0,
    0, 0, 0, 1, 0, 1, 0,
    0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 1, 0,
    0, 1, 0, 0, 0, 1, 0,
    0, 0, 1, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0
};
static Ili9341 display = Ili9341::ForSerial8Bit4Wire(displaySpi,
    chipSelectPort, chipSelectPin,
    dataCommandSelectPort, dataCommandSelectPin,
    connectionModeSelectPort, connectionModePins,
    origin, dimensions, Orientation::Landscape);

int main(void)
{
    SystemClock_Config();
    ApplicationInit();

    display.Init();

    DrawOnDisplay(display);

    while (1)
    {}
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
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN |
        RCC_AHB1ENR_GPIODEN |
        RCC_AHB1ENR_GPIOFEN;
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
}

void DrawOnDisplay(Ili9341 &display)
{
    for(int32_t i = 0; i < 0x10000; i+=31)
    {
        display.FillBackground(i);
        display.DrawFrame();
    }
    display.FillBackground(0x3333);
    display.DrawFrame();

    display.FillRectangle({50, 100}, {70, 40}, 0x5555);
    display.DrawSymbol(x, {70, 115}, {7, 12}, Color565::White, Color565::Black);
    display.DrawSymbol(y, {78, 115}, {7, 12}, Color565::White, Color565::Black);
    display.DrawSymbol(x, {86, 115}, {7, 12}, Color565::White, Color565::Black);

    display.DrawLine(Color565::Cyan, {10, 10}, {100, 230});
    display.DrawLine(Color565::Cyan, {11, 10}, {101, 230});
    display.DrawLine(Color565::Cyan, {12, 10}, {102, 230});
    display.DrawLine(Color565::Magenta, {100, 10}, {10, 230});
    display.DrawLine(Color565::Magenta, {101, 10}, {11, 230});
    display.DrawLine(Color565::Magenta, {102, 10}, {12, 230});
    
    display.DrawLine(Color565::Red, {110, 10}, {155, 120});
    display.DrawLine(Color565::Red, {111, 10}, {156, 120});
    display.DrawLine(Color565::Red, {112, 10}, {157, 120});
    display.DrawLine(Color565::Green, {200, 10}, {110, 230});
    display.DrawLine(Color565::Green, {201, 10}, {111, 230});
    display.DrawLine(Color565::Green, {202, 10}, {112, 230});
    
    display.DrawLine(Color565::Blue, {210, 10}, {210, 230});
    display.DrawLine(Color565::Blue, {211, 10}, {211, 230});
    display.DrawLine(Color565::Blue, {212, 10}, {212, 230});
    display.DrawLine(Color565::Yellow, {300, 10}, {210, 230});
    display.DrawLine(Color565::Yellow, {301, 10}, {211, 230});
    display.DrawLine(Color565::Yellow, {302, 10}, {212, 230});
    display.DrawLine(Color565::Cyan, {300, 10}, {300, 230});
    display.DrawLine(Color565::Cyan, {301, 10}, {301, 230});
    display.DrawLine(Color565::Cyan, {302, 10}, {302, 230});
    display.DrawFrame();
}
