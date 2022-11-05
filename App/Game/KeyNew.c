#include "stm32f10x_conf.h"
#include "Game.h"
#include "stdio.h"

int PressKey = 0;  //独立按键使用变量
int PressFlag = 0; //独立按键使用变量

/**
 * @brief 初始化独立按键
 *
 */
void KeyInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; // GPIO_InitStructure用于存放GPIO的参数

    //使能RCC相关时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIOA的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //使能GPIOC的时钟

    //配置PC1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;     //设置引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置输入类型
    GPIO_Init(GPIOC, &GPIO_InitStructure);        //根据参数初始化GPIO

    //配置PC2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;     //设置引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置输入类型
    GPIO_Init(GPIOC, &GPIO_InitStructure);        //根据参数初始化GPIO

    //配置PA0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;     //设置引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置输入类型
    GPIO_Init(GPIOA, &GPIO_InitStructure);        //根据参数初始化GPIO
}

void KeyCheck(void)
{
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1) == 0)
            PressKey = 1;
        else if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == 0)
            PressKey = 2;
        else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
            PressKey = 3;
        else
            PressKey = 0;
}

/**
 * @brief 按键按下弹起检测及按键功能定义
 *
 */
void KeyFunction(void)
{
    if (PressKey != 0)
    {
        if (PressFlag == 0)
        {
            // 1键为向左移动
            if (PressKey == 1)
            {
                LastPlayerLocation = PlayerLocation;
                PlayerLocation--;
                printf("You move to location %d\n", PlayerLocation);
                // printf("[Younger] You press Key %d, now your Location is %d", PressKey, PlayerLocation);
            }

            // 3键为向右移动
            else if (PressKey == 3)
            {
                LastPlayerLocation = PlayerLocation;
                PlayerLocation++;
                printf("You move to location %d\n", PlayerLocation);
                // printf("[Younger] You press Key %d, now your Location is %d", PressKey, PlayerLocation);
            }

            // 2键为发射子弹
            else if (PressKey == 2)
            {
                if (PlayerFire(PlayerLocation))
                    printf("[Younger] You press FIRE! You STILL have %d ammo(s)\n", Bullet);
                else
                    printf("[Younger] You press FIRE but you have NO ammo\n");
            }

            //玩家位置纠正
            if (PlayerLocation > 2)
                PlayerLocation = 2;
            if (PlayerLocation < 0)
                PlayerLocation = 0;

            PlayerMove(PlayerLocation, LastPlayerLocation);
            PressFlag = PressKey;
        }
    }
    else
    {
        if (PressFlag != 0)
        {
            // printf("[Younger] You release Key %d", PressFlag);
            PressFlag = 0;
        }
    }
}
