#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"

#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160

#define ST7735_RST_LOW()   GPIO_ResetBits(GPIOB, GPIO_Pin_10) // PB10
#define ST7735_RST_HIGH()  GPIO_SetBits(GPIOB, GPIO_Pin_10)

#define ST7735_CS_LOW()    GPIO_ResetBits(GPIOB, GPIO_Pin_12) // PB12
#define ST7735_CS_HIGH()   GPIO_SetBits(GPIOB, GPIO_Pin_12)

#define ST7735_DC_CMD()    GPIO_ResetBits(GPIOB, GPIO_Pin_14) // PB14
#define ST7735_DC_DATA()   GPIO_SetBits(GPIOB, GPIO_Pin_14)

#define ST7735_BLACK   0x0000
#define ST7735_BLUE    0x001F
#define ST7735_RED     0xF800
#define ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF

static const uint8_t Font5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Space
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // )
    {0x14, 0x08, 0x3E, 0x08, 0x14}, // *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // @
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x10, 0x28, 0x44, 0x00}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x26, 0x49, 0x49, 0x49, 0x32}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x3F, 0x40, 0x38, 0x40, 0x3C}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}  // Z
};

void Delay_ms(volatile uint32_t ms) {
    uint32_t count = ms * 7200;
    while (count--) {
        __NOP();
    }
}

void SPI2_Init_Hardware(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    SPI_Init(SPI2, &SPI_InitStructure);
    SPI_Cmd(SPI2, ENABLE);

    ST7735_CS_HIGH();
}

void SPI2_WriteByte(uint8_t data) {
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2, data);
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);
}

void ST7735_WriteCommand(uint8_t cmd) {
    ST7735_DC_CMD();
    ST7735_CS_LOW();
    SPI2_WriteByte(cmd);
    ST7735_CS_HIGH();
}

void ST7735_WriteData(uint8_t data) {
    ST7735_DC_DATA();
    ST7735_CS_LOW();
    SPI2_WriteByte(data);
    ST7735_CS_HIGH();
}

void ST7735_WriteData16(uint16_t data) {
    ST7735_DC_DATA();
    ST7735_CS_LOW();
    SPI2_WriteByte(data >> 8);
    SPI2_WriteByte(data & 0xFF);
    ST7735_CS_HIGH();
}

void ST7735_Init(void) {
    SPI2_Init_Hardware();

    ST7735_CS_LOW();
    ST7735_RST_HIGH(); Delay_ms(10);
    ST7735_RST_LOW();  Delay_ms(50);
    ST7735_RST_HIGH(); Delay_ms(120);

    ST7735_WriteCommand(0x01); Delay_ms(120); // SWRESET
    ST7735_WriteCommand(0x11); Delay_ms(120); // SLPOUT

    ST7735_WriteCommand(0xB1);
    ST7735_WriteData(0x01); ST7735_WriteData(0x2C); ST7735_WriteData(0x2D);

    ST7735_WriteCommand(0xC0);
    ST7735_WriteData(0x02); ST7735_WriteData(0x02); ST7735_WriteData(0x84);

    ST7735_WriteCommand(0xC1); 
    ST7735_WriteData(0xC5);   

    ST7735_WriteCommand(0xC2); 
    ST7735_WriteData(0x0A); ST7735_WriteData(0x00);

    ST7735_WriteCommand(0xC3); 
    ST7735_WriteData(0x8A); ST7735_WriteData(0x2A);

    ST7735_WriteCommand(0xC4); 
    ST7735_WriteData(0x8A); ST7735_WriteData(0xEE);

    ST7735_WriteCommand(0xC5);
    ST7735_WriteData(0x0E);  

    ST7735_WriteCommand(0x3A); ST7735_WriteData(0x05); 
    ST7735_WriteCommand(0x36); ST7735_WriteData(0xC8); 

    ST7735_WriteCommand(0xE0); // Positive Gamma
    ST7735_WriteData(0x02); ST7735_WriteData(0x1C); ST7735_WriteData(0x07); ST7735_WriteData(0x12);
    ST7735_WriteData(0x37); ST7735_WriteData(0x32); ST7735_WriteData(0x29); ST7735_WriteData(0x2D);
    ST7735_WriteData(0x29); ST7735_WriteData(0x25); ST7735_WriteData(0x2B); ST7735_WriteData(0x39);
    ST7735_WriteData(0x00); ST7735_WriteData(0x01); ST7735_WriteData(0x03); ST7735_WriteData(0x10);

    ST7735_WriteCommand(0xE1); // Negative Gamma
    ST7735_WriteData(0x03); ST7735_WriteData(0x1D); ST7735_WriteData(0x07); ST7735_WriteData(0x06);
    ST7735_WriteData(0x2E); ST7735_WriteData(0x2C); ST7735_WriteData(0x29); ST7735_WriteData(0x2D);
    ST7735_WriteData(0x2E); ST7735_WriteData(0x2E); ST7735_WriteData(0x37); ST7735_WriteData(0x3F);
    ST7735_WriteData(0x00); ST7735_WriteData(0x00); ST7735_WriteData(0x02); ST7735_WriteData(0x10);

    ST7735_WriteCommand(0x20); 
    ST7735_WriteCommand(0x29); Delay_ms(100); // Display ON
    Delay_ms(100);
}

void ST7735_SetAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    ST7735_WriteCommand(0x2A);
    ST7735_WriteData(0x00); ST7735_WriteData(x0);
    ST7735_WriteData(0x00); ST7735_WriteData(x1);

    ST7735_WriteCommand(0x2B);
    ST7735_WriteData(0x00); ST7735_WriteData(y0);
    ST7735_WriteData(0x00); ST7735_WriteData(y1);

    ST7735_WriteCommand(0x2C);
}

void ST7735_FillScreen(uint16_t color) {
    ST7735_SetAddrWindow(0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);
    for (uint32_t i = 0; i < ST7735_WIDTH * ST7735_HEIGHT; i++) {
        ST7735_WriteData16(color);
    }
}

void ST7735_DrawChar(uint8_t x, uint8_t y, char c, uint16_t color, uint16_t bg) {
    if (c < 32 || c > 126) c = ' ';
    uint8_t idx = c - 32;

    for (uint8_t i = 0; i < 5; i++) {
        uint8_t line = Font5x7[idx][i];
        for (uint8_t j = 0; j < 8; j++) {
            if (line & 0x01) {
                ST7735_SetAddrWindow(x + i, y + j, x + i, y + j);
                ST7735_WriteData16(color);
            } else if (bg != color) {
                ST7735_SetAddrWindow(x + i, y + j, x + i, y + j);
                ST7735_WriteData16(bg);
            }
            line >>= 1;
        }
    }
}

void ST7735_PrintString(uint8_t x, uint8_t y, const char* str, uint16_t color, uint16_t bg) {
    while (*str) {
        ST7735_DrawChar(x, y, *str++, color, bg);
        x += 6;
        if (x > ST7735_WIDTH - 6) {
            x = 0;
            y += 8;
        }
    }
}

int main(void) {
    ST7735_Init();
    ST7735_FillScreen(ST7735_BLACK);

    ST7735_PrintString(10, 10, "STM32F103 SPI2", ST7735_WHITE, ST7735_BLACK);
    ST7735_PrintString(10, 30, "TFT 1.8 ST7735", ST7735_WHITE, ST7735_BLACK);
    ST7735_PrintString(10, 50, "Man den chu trang", ST7735_WHITE, ST7735_BLACK);

    while (1) {
    }
}