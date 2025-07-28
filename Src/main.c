#include "stm32h7xx.h"

volatile uint8_t timer_flag = 0;

void GPIO_init(void);
void TIM_init(void);
void delay(int);

int main(void)
{
    GPIO_init();
    TIM_init();

    while (1)
    {
        GPIOE->BSRR = 0b1 << 3;
        delay(1000);
        GPIOE->BSRR = 0b1 << (3 + 16);
        delay(1000);
    }

    return 0;
}

void GPIO_init(void)
{
    // 初始化GPIOE时钟
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOEEN;

    // 设置E3
    // 输出
    GPIOE->MODER &= ~(0b11 << (2 * 3));
    GPIOE->MODER |= 0b01 << (2 * 3);
    // 推挽
    GPIOE->OTYPER &= ~(0b1 << 3);
    // 悬空
    GPIOE->PUPDR &= ~(0b11 << (2 * 3));
}

void TIM_init(void)
{
    // 初始化TIM6时钟
    RCC->APB1LENR |= RCC_APB1LENR_TIM6EN;

    // 设置预分频器，自动重载值
    TIM6->PSC = 64000 - 1;
    TIM6->ARR = 1000 - 1;

    // 设为OPM模式
    TIM6->CR1 = TIM_CR1_OPM;

    // 清除更新中断标志
    TIM6->SR &= ~TIM_SR_UIF;

    // 开启NVIC中断通道
    NVIC_EnableIRQ(TIM6_DAC_IRQn);

    // 使能中断
    TIM6->DIER |= TIM_DIER_UIE;
}

void delay(int n)
{
    // 清除更新中断标志
    TIM6->SR &= ~TIM_SR_UIF;

    timer_flag = 0;
    TIM6->ARR = n - 1;
    TIM6->CR1 |= TIM_CR1_CEN;

    while (!timer_flag)
        ;
}

void TIM6_DAC_IRQHandler(void)
{
    if (TIM6->SR & TIM_SR_UIF)
    {
        TIM6->SR &= ~TIM_SR_UIF;
        TIM6->CR1 &= ~TIM_CR1_CEN;
        timer_flag = 1;
    }
}