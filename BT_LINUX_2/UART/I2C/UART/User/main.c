#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

#define MA_LOP  "D21CQCN"
#define MA_NHOM "01"
#define BUFFER_SIZE 100

static char rx_buffer[BUFFER_SIZE];
static uint16_t rx_index = 0;

static void UART1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);
}

static void UART1_SendChar(char c)
{
    USART_SendData(USART1, (uint16_t)c);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

static void UART1_SendString(const char *str)
{
    while (*str)
    {
        UART1_SendChar(*str);
        str++;
    }
}

static void UART1_Process(void)
{
    uint16_t i;

    UART1_SendString(MA_LOP);
    UART1_SendString(MA_NHOM);
    UART1_SendString(": ");

    for (i = 0; i < rx_index; i++)
        UART1_SendChar(rx_buffer[i]);

    UART1_SendString("\r\n");
    rx_index = 0;
}

int main(void)
{
    char c;

    UART1_Config();
    UART1_SendString("UART READY\r\n");

    while (1)
    {
        if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
        {
            c = (char)USART_ReceiveData(USART1);

            if (c == '!')
            {
                UART1_Process();
            }
            else if (rx_index < BUFFER_SIZE - 1)
            {
                rx_buffer[rx_index] = c;
                rx_index++;
            }
        }
    }
}
