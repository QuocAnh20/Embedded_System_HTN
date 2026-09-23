#define RCC_APB2ENR     (*(volatile unsigned int *)0x40021018)

#define GPIOC_CRH       (*(volatile unsigned int *)0x40011004)
#define GPIOC_ODR       (*(volatile unsigned int *)0x4001100C)

void delay(int time )
{
    volatile unsigned int i,j;
    for (i = 0; i < time; i++){
     for(j=0;j<0x2aff;j++);
    }
}

int main(void)
{
    // Bật xung nhịp GPIOC (Bit 4)
    RCC_APB2ENR |= (1 << 4);

    // Cấu hình PC13 làm Output Push-Pull, 2 MHz
    GPIOC_CRH &= ~(0xF << 20);   // PC13 = bits 20-23
    GPIOC_CRH |=  (0x2 << 20);   // MODE13 = 10, CNF13 = 00

    while (1)
    {
        GPIOC_ODR ^= (1 << 13);  // Đảo trạng thái PC13
        delay(200);
        
    }
}
