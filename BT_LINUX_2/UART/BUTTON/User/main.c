#define RCC_APB2ENR  (*(volatile unsigned int *)(0x40021000 + 0x18))
#define GPIOB_CRL    (*(volatile unsigned int *)(0x40010C00 + 0x00))
#define GPIOB_IDR    (*(volatile unsigned int *)(0x40010C00 + 0x08))
#define GPIOB_BSRR   (*(volatile unsigned int *)(0x40010C00 + 0x10))
#define GPIOC_CRH    (*(volatile unsigned int *)(0x40011000 + 0x04))
#define GPIOC_ODR    (*(volatile unsigned int *)(0x40011000 + 0x0C))

void delay_ms(volatile unsigned int ms) {
    for (volatile unsigned int i = 0; i < ms * 1000; i++) {
    }
}

int main(void) {
    RCC_APB2ENR |= (1 << 3) | (1 << 4);

    GPIOB_CRL &= ~(0xF << 4);
    GPIOB_CRL |=  (0x8 << 4);
    GPIOB_BSRR =  (1 << 1); 

    GPIOC_CRH &= ~(0xF << 20);
    GPIOC_CRH |=  (0x2 << 20);

    GPIOC_ODR &= ~(1 << 13); 

    int button_last = 1;

    while (1) {
        int button_current = (GPIOB_IDR & (1 << 1)) ? 1 : 0;
        if (button_current == 0 && button_last == 1) {
            GPIOC_ODR ^= (1 << 13);
            delay_ms(100);
        }

        button_last = button_current;
    }
}
