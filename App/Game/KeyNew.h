#ifndef _KEYNEW_H_
#define _KEYNEW_H_

#include "DataType.h"
extern int PressKey;  //独立按键使用变量
extern int PressFlag; //独立按键使用变量

void KeyInit(void);
void KeyCheck(void);
void KeyFunction(void);

#endif
