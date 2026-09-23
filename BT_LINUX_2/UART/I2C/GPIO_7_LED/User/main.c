#include <stdint.h>

// 1. Định nghĩa cấu trúc thanh ghi GPIO (STM32F103)
typedef struct {
    volatile uint32_t CRL;   // Port configuration register low (PA0 - PA7)
    volatile uint32_t CRH;   // Port configuration register high (PA8 - PA15)
    volatile uint32_t IDR;   // Port input data register
    volatile uint32_t ODR;   // Port output data register
    volatile uint32_t BSRR;  // Port bit set/reset register
    volatile uint32_t BRR;   // Port bit reset register
    volatile uint32_t LCKR;  // Port configuration lock register
} GPIO_RegDef_t;

// 2. Định nghĩa địa chỉ Base của Peripherals
#define GPIOA_BASE  (0x40010800UL)
#define RCC_BASE    (0x40021000UL)

// 3. Con trỏ ép kiểu đến các thanh ghi
#define GPIOA       ((GPIO_RegDef_t *) GPIOA_BASE)
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x18UL))

// Hàm Delay đơn giản
void mdelay(volatile uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms; i++) {
        for (volatile uint32_t j = 0; j < 800; j++) {
            __asm__("nop"); // Giữ vòng lặp không bị compiler tối ưu xóa bỏ
        }
    }
}

int main(void)
{
    // 1. Bật xung Clock cho GPIOA (Set bit 2 của RCC_APB2ENR)
    RCC_APB2ENR |= (1 << 2);

    // 2. Cấu hình các chân PA0 -> PA7 làm Output Push-Pull 50MHz
    // Mỗi chân chiếm 4 bit (MODE = 11: Output 50MHz, CNF = 00: General purpose output push-pull)
    // 8 chân từ PA0 đến PA7 tương ứng giá trị Hex: 0x33333333
    GPIOA->CRL = 0x33333333;

    while (1)
    {
        // Quét LED xuôi: PA0 -> PA7
        for (int i = 0; i < 8; i++) {
            GPIOA->ODR = (1 << i);
            mdelay(200);
        }

        // Quét LED ngược: PA7 -> PA0
        for (int i = 7; i >= 0; i--) {
            GPIOA->ODR = (1 << i);
            mdelay(200);
        }
    }
}
