#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

volatile uint32_t cnt1 = 0;
volatile uint32_t cnt2 = 0;
volatile uint32_t cnt3 = 0;

static void LED_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOA, GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_7);
}

void SysTick_Handler(void)
{
    cnt1++;
    cnt2++;
    cnt3++;

    if (cnt1 >= 5000)
    {
        GPIOA->ODR ^= GPIO_Pin_3;
        cnt1 = 0;
    }

    if (cnt2 >= 500)
    {
        GPIOA->ODR ^= GPIO_Pin_5;
        cnt2 = 0;
    }

    if (cnt3 >= 50)
    {
        GPIOA->ODR ^= GPIO_Pin_7;
        cnt3 = 0;
    }
}

int main(void)
{
    SystemInit();
    LED_Config();

    SysTick_Config(SystemCoreClock / 1000);

    while (1)
    {
    }
}
