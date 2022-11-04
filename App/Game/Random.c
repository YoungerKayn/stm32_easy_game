#include "Game.h"
int seed = 1; //随机数种子

/**
 * @brief 生成随机数
 *
 * @return int 随机数
 */
int rand(void)
{
    seed = (seed * 214013 + 2531011) & 0x7fff;
    return seed;
}

/**
 * @brief 改变随机数种子
 *
 * @param seed 随机数种子变量地址
 */
void srand(int *seed)
{
    *seed = MicroSec + PlayerLocation + LastPlayerLocation + EnemyLocation[0][1] + EnemyLocation[1][1] + EnemyLocation[2][1];
}
