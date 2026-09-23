#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_usart.h"

static void Delay_ms(uint32_t ms)
{
    uint32_t i;
    uint32_t j;
    for (i = 0; i < ms; i++)
        for (j = 0; j < 0x2aff; j++);
}

static void ADC_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);

    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

static uint16_t ADC_Read(void)
{
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    return ADC_GetConversionValue(ADC1);
}

static void USART_Config(void)
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

static void USART_SendChar(char c)
{
    USART_SendData(USART1, c);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

static void USART_SendString(char *str)
{
    while (*str)
    {
        USART_SendChar(*str);
        str++;
    }
}

static void Send_Number(uint32_t value)
{
    char buf[10];
    int i;

    i = 0;

    if (value == 0)
    {
        USART_SendChar('0');
        return;
    }

    while (value > 0)
    {
        buf[i++] = (value % 10) + '0';
        value /= 10;
    }

    while (i > 0)
        USART_SendChar(buf[--i]);
}

int main(void)
{
    uint16_t adc_value;
    uint32_t voltage_mv;

    SystemInit();
    ADC_Config();
    USART_Config();

    USART_SendString("ADC STM32F103\r\n");

    while (1)
    {
        adc_value = ADC_Read();

        voltage_mv = ((uint32_t)adc_value * 3300) / 4095;

        USART_SendString("ADC = ");
        Send_Number(adc_value);
        USART_SendString(" | Voltage = ");

        Send_Number(voltage_mv / 1000);
        USART_SendChar('.');
        Send_Number((voltage_mv % 1000) / 100);
        Send_Number((voltage_mv % 100) / 10);
        Send_Number(voltage_mv % 10);

        USART_SendString(" V\r\n");

        Delay_ms(3000);
    }
}

