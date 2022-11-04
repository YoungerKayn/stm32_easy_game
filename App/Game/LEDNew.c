#include "stm32f10x_conf.h"

/**
 * @brief 初始化LED引脚和时钟
 *
 */
void LEDInit(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;

    // Pin 4 对应蓝色LED
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Pin 5 对应绿色LED
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_RESET);
    GPIO_WriteBit(GPIOC, GPIO_Pin_5, Bit_RESET);
}

/**
 * @brief 点亮指定的LED灯
 *
 * @param LEDType 0为蓝色灯,1为绿色灯
 * @param LEDState 0灭1亮
 */
void LEDSet(u8 LEDType, u8 LEDState)
{
    if (!LEDType)
    {
        if (LEDState)
            GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_SET);
        else
            GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_RESET);
    }
    else
    {
        if (LEDState)
            GPIO_WriteBit(GPIOC, GPIO_Pin_5, Bit_SET);
        else
            GPIO_WriteBit(GPIOC, GPIO_Pin_5, Bit_RESET);
    }
}

/**
 * @brief 根据LEDState调整LED显示方式
 *
 * @param LEDState
 */
void LEDSelector(int LEDState)
{
    switch (LEDState)
    {
    case 0:
        LEDSet(0, 0);
        LEDSet(1, 0);
        break;
    case 3:
        LEDSet(0, 0);
        LEDSet(1, 1);
        break;
    case 6:
        LEDSet(0, 0);
        LEDSet(1, 1);
        break;
    case 9:
        LEDSet(0, 1);
        LEDSet(1, 0);
        break;
    }
}
