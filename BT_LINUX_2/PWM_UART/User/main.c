#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

volatile char rx_cmd[32];
volatile uint8_t rx_index = 0;
volatile uint8_t cmd_ready = 0;

uint8_t led_state = 0;
uint8_t pwm_percent = 50;

void PWM_Config(void)
{
    GPIO_InitTypeDef gpio;
    TIM_TimeBaseInitTypeDef tim;
    TIM_OCInitTypeDef oc;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    tim.TIM_Prescaler = 71;
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_Period = 999;
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &tim);

    TIM_OCStructInit(&oc);
    oc.TIM_OCMode = TIM_OCMode_PWM1;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_OCPolarity = TIM_OCPolarity_High;
    oc.TIM_Pulse = 0;
    TIM_OC1Init(TIM2, &oc);

    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void UART_Config(void)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef uart;
    NVIC_InitTypeDef nvic;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    uart.USART_BaudRate = 115200;
    uart.USART_WordLength = USART_WordLength_8b;
    uart.USART_StopBits = USART_StopBits_1;
    uart.USART_Parity = USART_Parity_No;
    uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &uart);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    nvic.NVIC_IRQChannel = USART1_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    USART_Cmd(USART1, ENABLE);
}

void UART_SendChar(char c)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, c);
}

void UART_SendString(char *str)
{
    while (*str)
        UART_SendChar(*str++);
}

void LED_ON(void)
{
    led_state = 1;
    TIM_SetCompare1(TIM2, pwm_percent * 10);
}

void LED_OFF(void)
{
    led_state = 0;
    TIM_SetCompare1(TIM2, 0);
}

void PWM_Set(uint8_t percent)
{
    if (percent > 100)
        percent = 100;

    pwm_percent = percent;

    if (led_state)
        TIM_SetCompare1(TIM2, percent * 10);
}

void Process_Command(void)
{
    char cmd[32];
    char msg[40];
    int percent;

    strcpy(cmd, (char *)rx_cmd);

    if (strcmp(cmd, "ON") == 0)
    {
        LED_ON();
        UART_SendString("OK:ON!\r\n");
    }
    else if (strcmp(cmd, "OFF") == 0)
    {
        LED_OFF();
        UART_SendString("OK:OFF!\r\n");
    }
    else if (strcmp(cmd, "Status") == 0)
    {
        sprintf(msg, "LED:%s PWM:%d%%!\r\n",
                led_state ? "ON" : "OFF", pwm_percent);
        UART_SendString(msg);
    }
    else if (strncmp(cmd, "PWM:", 4) == 0)
    {
        percent = atoi(&cmd[4]);

        if (percent >= 0 && percent <= 100)
        {
            PWM_Set((uint8_t)percent);
            sprintf(msg, "OK:PWM:%d%%!\r\n", percent);
            UART_SendString(msg);
        }
        else
        {
            UART_SendString("ERROR:PWM!\r\n");
        }
    }
    else
    {
        UART_SendString("ERROR:CMD!\r\n");
    }
}

void USART1_IRQHandler(void)
{
    char c;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        c = USART_ReceiveData(USART1);

        if (c == '!')
        {
            rx_cmd[rx_index] = '\0';
            rx_index = 0;
            cmd_ready = 1;
        }
        else if (c != '\r' && c != '\n')
        {
            if (rx_index < 31)
                rx_cmd[rx_index++] = c;
            else
                rx_index = 0;
        }
    }
}

int main(void)
{
    PWM_Config();
    UART_Config();

    UART_SendString("READY!\r\n");

    while (1)
    {
        if (cmd_ready)
        {
            cmd_ready = 0;
            Process_Command();
        }
    }
}
