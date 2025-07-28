#include "stm32h7xx.h"

typedef void (*pAppEntry)(void);
void SwitchApp(uint32_t appAddress);

void AppChoose(void)
{
    // 初始化时钟
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;

    // 设置C13的状态（按键）
    GPIOC->MODER &= ~(0b11 << (13 * 2));

    GPIOC->PUPDR |= (0b10 << (13 * 2));

    volatile uint8_t ispressed = 0;

    if (GPIOC->IDR & (0b1 << 13))
    {
        ispressed = 1;
    }

    // 重置时钟
    RCC->AHB4ENR &= ~RCC_AHB4ENR_GPIOCEN;

    if (ispressed == 1)
    {
        SwitchApp(0x08020000);
    }
}

void SwitchApp(uint32_t appAddress)
{
    uint32_t mspValue = *(uint32_t *)appAddress;
    uint32_t resetVector = *(uint32_t *)(appAddress + 4);

    // 关闭中断
    __disable_irq();

    // 设置中断向量表基地址
    SCB->VTOR = appAddress;

    // 设置主栈指针
    __set_MSP(mspValue);

    // 跳转到应用程序的复位入口
    pAppEntry appEntry = (pAppEntry)resetVector;

    // 重新使能中断
    __enable_irq();

    appEntry();

    // 理论上不会执行到这里
    while (1)
        ;
}
