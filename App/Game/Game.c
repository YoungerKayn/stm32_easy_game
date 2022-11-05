#include "stm32f10x_conf.h"
#include "DataType.h"
#include "OLED.h"
#include "LEDNew.h"
#include "Random.h"
#include <stdio.h>

#define SpeedLevel 10 //游戏速度等级, 越小游戏越快

i16 MicroSec = 0;                                         //游戏运行时间
i16 Difficulty = 1;                                       //难度
int Score = 0;                                            //分数
int Bullet = 3;                                           //初始子弹数量
int PlayerLocation = 1;                                   //玩家当前所在位置
int LastPlayerLocation = 1;                               //玩家上一个所在位置
int PlayerCrashFlag = 0;                                  //玩家死亡标志位
int EnemyLocation[3][2] = {{0, -16}, {0, -16}, {0, -16}}; //敌机所在道路及位置

void InitGame(void);
void GameProcess(void);
void GameOverEffect(int EffectLocation, int FillFlag);
void PlayerShow(int PlayerLocation);
void PlayerMove(int PlayerLocation, int LastPlayerLocation);
int PlayerFire(int PlayerLocation);
void PlayerCrash(int CrashLocation);
int EnemyMove(void);
void MapMove(void);

/******************************************************************/

/**
 * @brief 延迟 {xms} ms
 *
 * @param xms
 */
void Delay_ms(uint32_t xms)
{
    __IO uint32_t i = 0;
    while (xms--)
    {
        i = 10000;
        while (i--)
            ;
    }
}

/**
 * @brief 将传入的数字转化为字符串后赋值给传入的字符串
 *
 * @param num 待转化的数字
 * @param str 待赋值的字符串
 * @param radix 数字的进制
 * @return char* 转换后的字符串
 */
char *itoa(int num, char *str, int radix)
{
    char index[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; //索引表
    unsigned unum;                                         //存放要转换的整数的绝对值,转换的整数可能是负数
    int i = 0, j, k;                                       // i用来指示设置字符串相应位，转换之后i其实就是字符串的长度；转换后顺序是逆序的，有正负的情况，k用来指示调整顺序的开始位置;j用来指示调整顺序时的交换。

    //获取要转换的整数的绝对值
    if (radix == 10 && num < 0) //要转换成十进制数并且是负数
    {
        unum = (unsigned)-num; //将num的绝对值赋给unum
        str[i++] = '-';        //在字符串最前面设置为'-'号，并且索引加1
    }
    else
        unum = (unsigned)num; //若是num为正，直接赋值给unum

    //转换部分，注意转换后是逆序的
    do
    {
        str[i++] = index[unum % (unsigned)radix]; //取unum的最后一位，并设置为str对应位，指示索引加1
        unum /= radix;                            // unum去掉最后一位

    } while (unum); //直至unum为0退出循环

    str[i] = '\0'; //在字符串最后添加'\0'字符，c语言字符串以'\0'结束。

    //将顺序调整过来
    if (str[0] == '-')
        k = 1; //如果是负数，符号不用调整，从符号后面开始调整
    else
        k = 0; //不是负数，全部都要调整

    char temp;                         //临时变量，交换两个值时用到
    for (j = k; j <= (i - 1) / 2; j++) //头尾一一对称交换，i其实就是字符串的长度，索引最大值比长度少1
    {
        temp = str[j];               //头部赋值给临时变量
        str[j] = str[i - 1 + k - j]; //尾部赋值给头部
        str[i - 1 + k - j] = temp;   //将临时变量的值(其实就是之前的头部值)赋给尾部
    }

    return str; //返回转换后的字符串
}

/******************************************************************/

/******************************************************************/

/**
 * @brief 初始化游戏进程
 *
 */
void InitGame(void)
{
    Difficulty = 1;         //难度
    Score = 0;              //分数
    Bullet = 3;             //初始子弹数量
    PlayerLocation = 1;     //玩家当前所在位置
    LastPlayerLocation = 1; //玩家上一个所在位置
    PlayerCrashFlag = 0;    //玩家死亡标志位

    printf(" [Younger] You start a new game, good luck!\n");

    srand(Difficulty); //初始化随机数队列

    //初始化地图
    OLEDShowString(0, 0, "|    |    |    |");
    OLEDShowString(0, 16, "|    |    |    |");
    OLEDShowString(0, 32, "|    |    |    |");
    OLEDShowString(0, 48, "|    |    |    |");

    //初始化玩家位置
    OLEDShowString(56, 48, "\\^");
}

/**
 * @brief 游戏进程
 *
 */
void GameProcess(void)
{
    static i16 DifficultyCounter = 0, ScoreCounter = 0;
    static i16 PlayerShowCounter = 0, EnemyMoveCounter = 0, RoadMoveCounter = 0;
    static i16 BulletAddCounter = 0, BulletEddectCounter = 0;

    int NewSeed = MicroSec + Difficulty + PlayerLocation + LastPlayerLocation + EnemyLocation[0][1] + EnemyLocation[1][1] + EnemyLocation[2][1];
    srand(NewSeed);

    int i;

    if (!PlayerCrashFlag) //没有被碰撞, 运行游戏进程相关代码
    {
        PlayerShowCounter++;
        if (PlayerShowCounter > 50)
            PlayerShow(PlayerLocation);

        //每过2s难度增加
        DifficultyCounter++;
        if (DifficultyCounter > 1000)
        {
            printf("Speed up!\n");
            Difficulty++;
            DifficultyCounter = 0;
        }

        //每0.5s分数增加
        ScoreCounter++;
        if (ScoreCounter > 250)
        {
            Score++;
            ScoreCounter = 0;
        }

        //每5s获得一颗子弹
        BulletAddCounter++;
        if (BulletAddCounter > 2500)
        {
            Bullet++;
            printf("You get an ammo. Now you have %d ammo(s)\n", Bullet);
            BulletAddCounter = 0;
        }

        //随机出现一路敌人
				int AttackProbility = 499; //敌人不出现的可能性
        int EnemyAttack = rand() % AttackProbility;
        if (EnemyAttack < 3)
            EnemyLocation[EnemyAttack][0] = 1;

        //敌人前进
        EnemyMoveCounter += 2 * Difficulty;
        while (EnemyMoveCounter > SpeedLevel)
        {
            for (i = 0; i < 3; i++)
                if (EnemyLocation[i][0])
                    EnemyLocation[i][1]++;
            if (EnemyMove())
            {
                PlayerCrashFlag = 1;
                PlayerCrash(PlayerLocation);
            }
            EnemyMoveCounter -= SpeedLevel;
        }

        //动态地图绘制
        RoadMoveCounter += 4 * Difficulty;
        if (RoadMoveCounter > SpeedLevel)
        {
            MapMove();
            RoadMoveCounter = 0;
        }
    }
    else //碰撞后, 运行游戏结束相关代码
    {
        static int EffectLocation = 0, EffectCounter = 0, EffectFlag = 1;

        char ScoreStr[3];          //字符串形式的分数
        itoa(Score, ScoreStr, 10); //将分数转化为字符串形式以便用于显示

        EffectCounter++;
        if (EffectCounter > 0)
        {
            GameOverEffect(EffectLocation, EffectFlag); //游戏结束动画
            LEDSelector((EffectLocation + 1) % 16);     // LED氛围灯
            EffectLocation++;
            if (EffectLocation > 127)
                if (EffectFlag) // EffectFlag用于使动画以取反的形式再显示一次
                {
                    EffectFlag = 0;
                    EffectLocation = 0;
                }
                else //显示完两次动画后显示分数
                {
                    EffectLocation = 127;
                    OLEDShowString(32, 0, "Score:");
                    OLEDShowString(80, 0, ScoreStr);
                    OLEDShowStringBySize12(24, 40, "Press RST");
                    OLEDShowStringBySize12(20, 52, "to restart");
                }
            EffectCounter = 0;
        }
    }
}

/**
 * @brief 播放死亡动画及分数, 需由定时器驱动
 *
 * @param EffectLocation 动画显示位置
 * @param FillFlag 动画显示方式
 */
void GameOverEffect(int EffectLocation, int FillFlag)
{
    int i;
    for (i = 0; i < 128; i++)
        OLEDDrawPointForMap(i, EffectLocation, FillFlag);
}

/******************************************************************/

/******************************************************************/

/**
 * @brief 刷新玩家的显示
 *
 * @param PlayerLocation 玩家当前位置
 */
void PlayerShow(int PlayerLocation)
{
    //将玩家显示在当前位置
    switch (PlayerLocation)
    {
    case 0:
        OLEDShowString(16, 48, "\\^");
        break;
    case 1:
        OLEDShowString(56, 48, "\\^");
        break;
    case 2:
        OLEDShowString(96, 48, "\\^");
        break;
    default:
        break;
    }
}

/**
 * @brief 当玩家移动时, 刷新玩家位置
 *
 * @param PlayerLocation 玩家当前位置
 * @param LastPlayerLocation 玩家上一个位置
 */
void PlayerMove(int PlayerLocation, int LastPlayerLocation)
{
    if (PlayerCrashFlag)
        return;
    //清空玩家上一个所在位置
    switch (LastPlayerLocation)
    {
    case 0:
        OLEDShowString(16, 48, "  ");
        break;
    case 1:
        OLEDShowString(56, 48, "  ");
        break;
    case 2:
        OLEDShowString(96, 48, "  ");
        break;
    default:
        break;
    }
    //将玩家显示在当前位置
    switch (PlayerLocation)
    {
    case 0:
        OLEDShowString(16, 48, "\\^");
        break;
    case 1:
        OLEDShowString(56, 48, "\\^");
        break;
    case 2:
        OLEDShowString(96, 48, "\\^");
        break;
    default:
        break;
    }
}

/**
 * @brief 发射子弹摧毁玩家所在道路上的敌机
 *
 * @param PlayerLocation 玩家所在道路
 * @return int 发射成功返回1, 否则返回0
 */
int PlayerFire(int PlayerLocation)
{
    if (Bullet > 0)
    {
        int i;

        //发射动画1
        for (i = 0; i < 48; i++)
        {
            OLEDDrawPointForMap(23 + PlayerLocation * 40, i, 1);
            OLEDDrawPointForMap(24 + PlayerLocation * 40, i, 1);
        }
        OLEDRefreshGRAM();
        EnemyLocation[PlayerLocation][0] = 0; //清零当前道路的敌机存在标志位
        Bullet--;                             //子弹数减1
        Delay_ms(100);
        //发射动画2
        for (i = 0; i < 64; i++)
        {
            OLEDDrawPointForMap(23 + PlayerLocation * 40, i, 0);
            OLEDDrawPointForMap(24 + PlayerLocation * 40, i, 0);
        }
        return 1; //发射成功
    }
    else
        return 0; //发射失败
}

/**
 * @brief 在玩家死亡位置显示"GG", 并通过串口输出死亡信息及分数
 *
 * @param CrashLocation 玩家死亡位置
 */
void PlayerCrash(int CrashLocation)
{
    OLEDShowString(16 + CrashLocation * 40, 48, "GG");
    OLEDRefreshGRAM();
    printf("Your Plane was crashed...\nYour score: %d\nPress RESET to start a new game.\n", Score);
}
/******************************************************************/

/******************************************************************
 *关于敌机和地图的绘制
 */

/**
 * @brief 刷新敌机位置
 *
 * @return int
 */
int EnemyMove(void)
{
    int i;
    for (i = 0; i < 3; i++)
    {
        if (EnemyLocation[i][0]) //检测当前道路是否有敌人
        {
            OLEDShowString(16 + i * 40, EnemyLocation[i][1] - 1, "  "); //清空敌机所处的前一个位置
            OLEDShowString(16 + i * 40, EnemyLocation[i][1], "_`");     //在敌机当前位置显示

            if (EnemyLocation[i][1] >= 34 && EnemyLocation[i][1] <= 56) //检测是否碰撞
            {
                if (PlayerLocation == i)
                    return 1;
            }
            else if (EnemyLocation[i][1] >= 80) //敌机到达屏幕最底端时清空显示
            {
                OLEDShowString(16 + i * 40, EnemyLocation[i][1], "  ");
                EnemyLocation[i][0] = 0;   //将当前道路标记为没有敌机
                EnemyLocation[i][1] = -16; //将当前道路敌机位置清零
            }
        }
        else //当前道路没有敌人或敌人被消除(用子弹), 则清零当前敌机位置
        {
            OLEDShowString(16 + i * 40, EnemyLocation[i][1], "  ");
            EnemyLocation[i][1] = -16; //将当前道路敌机位置清零
        }
    }
    return 0; //没有碰撞
}

/**
 * @brief 绘制动态地图
 *
 */
void MapMove(void)
{
    /**
     * 原理: 将长为64像素的道路分割线分割为4段, 每段长16像素,
     * 每段各有四个空像素点, 它们相对被分割的道路的起点的距离相同,
     * 使四个空像素点与起点的距离增加, 像素点向下移动, 视觉上表现为道路向后移动
     */
    int RoadCount = 4; //道路分割线数量

    static int RoadLocation = 0;
    int Road;

    RoadLocation++;
    for (Road = RoadCount; Road >= 0; Road--)
    {
        int i, j;
        for (i = 0; i < 4; i++)
        {
            OLEDDrawPointForMap(4 + 40 * Road, RoadLocation - 4 + 16 * i, 1);
            for (j = 0; j < 4; j++)
                OLEDDrawPointForMap(4 + 40 * Road, RoadLocation - j + 16 * i, 0);
        }
    }

    if (RoadLocation >= 16) //到达被分割的道路的终点, 回到起点进行循环
        RoadLocation = 0;
}
