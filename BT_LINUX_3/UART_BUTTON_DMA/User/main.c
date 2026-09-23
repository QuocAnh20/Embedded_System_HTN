#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"
#include <stdio.h>
#include <string.h>

#define TX_BUF_SIZE 64

char tx_buffer[TX_BUF_SIZE];
uint32_t btn_count = 0;

void Delay_ms(volatile uint32_t ms) {
    uint32_t count = ms * 7200;
    while (count--) {
        __NOP();
    }
}

void Button_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void USART1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

    USART_Cmd(USART1, ENABLE);
}

void DMA_USART1_Init(void) {
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)tx_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 0;          
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
}

void UART_Send_DMA(const char* str) {
    uint16_t len = strlen(str);
    if (len >= TX_BUF_SIZE) len = TX_BUF_SIZE - 1;

    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA_ClearFlag(DMA1_FLAG_TC4);

    memcpy(tx_buffer, str, len);

    DMA1_Channel4->CNDTR = len;
    DMA_Cmd(DMA1_Channel4, ENABLE);
}

int main(void) {
    uint8_t btn_last_state = 1;
    uint8_t btn_curr_state = 1;

    Button_Init();
    USART1_Init();
    DMA_USART1_Init();

    while (1) {
        btn_curr_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);

        if (btn_last_state == 1 && btn_curr_state == 0) {
            Delay_ms(20); 
            if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0) {
                btn_count++;

                char temp_str[TX_BUF_SIZE];
                sprintf(temp_str, "D23DCDT017-N03:BTN:%lu\n\r", btn_count);

                UART_Send_DMA(temp_str);
            }
        }
        btn_last_state = btn_curr_state;
    }
}
