#ifndef _GAME_H_
#define _GAME_H_

#include "DataType.h"

extern i16 MicroSec;            //游戏运行时间
extern i16 Difficulty;          //难度
extern int Score;               //分数
extern int Bullet;              //初始子弹数量
extern int PlayerLocation;      //玩家当前所在位置
extern int LastPlayerLocation;  //玩家上一个所在位置
extern int PlayerCrashFlag;     //玩家死亡标志位
extern int EnemyLocation[3][2]; //敌机所在道路及位置

void InitGame(void);
void GameProcess(void);
int PlayerFire(int PlayerLocation);
void PlayerMove(int PlayerLocation, int LastPlayerLocation);

#endif
