#include "stm32f4xx.h"
#include "config.h"
#include "Ili9341.hpp"

void SystemClock_Config(void);
void ApplicationInit(void);
void DrawOnDisplay(Ili9341 &display);
void ApplicationDeInit(void);
void ApplicationInit2(void);

static SPI_TypeDef *displaySpi = SPI5;
static GPIO_TypeDef *chipSelectPort = GPIOC;
static uint8_t chipSelectPin = 2;
static GPIO_TypeDef *dataCommandSelectPort = GPIOD;
static uint8_t dataCommandSelectPin = 13;
static GPIO_TypeDef *spiPort = GPIOF;
static GPIO_TypeDef *connectionModeSelectPort = GPIOD;
static uint8_t connectionModePins[4] = {2, 4, 5, 7};

static GPIO_TypeDef *readStrobePort = GPIOD;
static uint8_t readStrobePin = 12;
static GPIO_TypeDef *writeStrobePort = GPIOD;
static uint8_t writeStrobePin = 13;
static GPIO_TypeDef *dPorts[16] = {
    GPIOD, GPIOG, GPIOG, GPIOA,
    GPIOB, GPIOB, GPIOA, GPIOG,
    GPIOB, GPIOB, GPIOC, GPIOD,
    GPIOC, GPIOB, GPIOA, GPIOA
};
static uint8_t dPins[16] = {
    6, 11, 12, 3,
    8, 9, 6, 10,
    10, 11, 7, 3,
    10, 0, 11, 12
};

int main(void)
{
    SystemClock_Config();
    ApplicationInit();

    // Ili9341 display = Ili9341::ForSerial8Bit4Wire(displaySpi,
    //     chipSelectPort, chipSelectPin,
    //     dataCommandSelectPort, dataCommandSelectPin,
    //     connectionModeSelectPort, connectionModePins,
    //     { 0, 0 },
    //     320, 240,
    //     ColorMode::R5G6B5,
    //     Orientation::Landscape);

    // display.Init();
    
    // DrawOnDisplay(display);

    // ApplicationDeInit();

    dataCommandSelectPort = GPIOF;
    dataCommandSelectPin = 7;
    
    ApplicationInit2();

    Ili9341 display = Ili9341::ForParallel16Bit(
        dPorts, dPins,
        chipSelectPort, chipSelectPin,
        dataCommandSelectPort, dataCommandSelectPin,
        readStrobePort, readStrobePin,
        writeStrobePort, writeStrobePin,
        connectionModeSelectPort, connectionModePins,
        { 0, 0 },
        320, 240,
        ColorMode::R5G6B5,
        Orientation::Landscape);

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
    display.FillBackground(Color565::Magenta);

    display.DrawLine(Color565::Cyan, {10, 10}, {100, 230});
    display.DrawLine(Color565::Cyan, {11, 10}, {101, 230});
    display.DrawLine(Color565::Cyan, {12, 10}, {102, 230});
    display.DrawLine(Color565::Cyan, {100, 10}, {10, 230});
    display.DrawLine(Color565::Cyan, {101, 10}, {11, 230});
    display.DrawLine(Color565::Cyan, {102, 10}, {12, 230});
    
    display.DrawLine(Color565::Cyan, {110, 10}, {155, 120});
    display.DrawLine(Color565::Cyan, {111, 10}, {156, 120});
    display.DrawLine(Color565::Cyan, {112, 10}, {157, 120});
    display.DrawLine(Color565::Cyan, {200, 10}, {110, 230});
    display.DrawLine(Color565::Cyan, {201, 10}, {111, 230});
    display.DrawLine(Color565::Cyan, {202, 10}, {112, 230});
    
    display.DrawLine(Color565::Cyan, {210, 10}, {210, 230});
    display.DrawLine(Color565::Cyan, {211, 10}, {211, 230});
    display.DrawLine(Color565::Cyan, {212, 10}, {212, 230});
    display.DrawLine(Color565::Cyan, {300, 10}, {210, 230});
    display.DrawLine(Color565::Cyan, {301, 10}, {211, 230});
    display.DrawLine(Color565::Cyan, {302, 10}, {212, 230});
    display.DrawLine(Color565::Cyan, {300, 10}, {300, 230});
    display.DrawLine(Color565::Cyan, {301, 10}, {301, 230});
    display.DrawLine(Color565::Cyan, {302, 10}, {302, 230});
}

void ApplicationDeInit(void)
{
    displaySpi->CR1 &= ~(SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE);
    RCC->APB2ENR &= ~RCC_APB2ENR_SPI5EN;

    spiPort->AFR[1] = 0;
    spiPort->AFR[0] = 0;
    spiPort->OSPEEDR = 0;
    spiPort->MODER = 0;
    
    dataCommandSelectPort->OSPEEDR = 0;
    dataCommandSelectPort->MODER = 0;
        
    RCC->AHB1ENR = 0x00100000;
}

void ApplicationInit2(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN |
        RCC_AHB1ENR_GPIOBEN |
        RCC_AHB1ENR_GPIOCEN |
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

    readStrobePort->BSRR = (1 << readStrobePin);
    readStrobePort->MODER |= (1 << (readStrobePin << 1));
    readStrobePort->OSPEEDR |= (3 << (readStrobePin << 1));

    writeStrobePort->BSRR = (1 << (writeStrobePin + 16));
    writeStrobePort->MODER |= (1 << (writeStrobePin << 1));
    writeStrobePort->OSPEEDR |= (3 << (writeStrobePin << 1));
    
    for (size_t i = 0; i < sizeof(dPins); ++i)
    {
        dPorts[i]->MODER |= (1 << (dPins[i] << 1));
        dPorts[i]->OSPEEDR |= (3 << (dPins[i] << 1));
    }
}
