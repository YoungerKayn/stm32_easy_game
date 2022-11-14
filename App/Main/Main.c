/*********************************************************************************************************
 *                                              包含头文件
 *********************************************************************************************************/
#include "Main.h"
#include "stm32f10x_conf.h"
#include "DataType.h"
#include "NVIC.h"
#include "SysTick.h"
#include "RCC.h"
#include "Timer.h"
#include "UART1.h"
#include "OLED.h"
#include "RunClock.h"

#include "LEDNew.h"
#include "KeyNew.h"
#include "Game.h"

/*********************************************************************************************************
 *                                              内部变量
 *********************************************************************************************************/
i16 ControlFlag = 0;

/*********************************************************************************************************
 *                                              内部函数声明
 *********************************************************************************************************/
static void InitSoftware(void); //初始化软件相关的模块
static void InitHardware(void); //初始化硬件相关的模块
static void Proc2msTask(void);  // 2ms处理任务
static void Proc1SecTask(void); // 1s处理任务

/*********************************************************************************************************
 *                                              内部函数实现
 *********************************************************************************************************/
static void InitHardware(void)
{
    SystemInit();      //系统初始化
    InitRCC();         //初始化RCC模块
    InitNVIC();        //初始化NVIC模块
    InitUART1(115200); //初始化UART模块
    InitTimer();       //初始化Timer模块
    InitSysTick();     //初始化SysTick模块
    InitOLED();        //初始化OLED模块

    LEDInit();
    KeyInit();
}

static void Proc2msTask(void)
{
    if (Get2msFlag()) //判断2ms标志状态
    {
        MicroSec += 2;     //游戏时间
        GameProcess();     //游戏进程
        KeyCheck();        //按键输入检测
        KeyFunction();     //独立按键检测及功能实现
        OLEDRefreshGRAM(); //刷新OLED屏显示

        Clr2msFlag();
    }
}

static void Proc1SecTask(void)
{

    if (Get1SecFlag())
    {
        Clr1SecFlag();
    }
}

/**
 * @brief 简单的小游戏
 *
 * @author Younger
 */
int main(void)
{
    InitHardware(); //初始化硬件相关函数
    InitGame();     //初始化游戏进程
    while (1)
    {
        Proc2msTask();
    }
}
