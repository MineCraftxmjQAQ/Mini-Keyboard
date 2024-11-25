#ifndef __CTRL_H__
#define __CTRL_H__

#include "main.h"

extern uint8_t KeyMapping_Mode;			//外部调用声明
extern uint8_t Luminancelimit_Mode;		//外部调用声明
extern uint8_t RBGChange_Mode;			//外部调用声明

/* 外部可调用函数 */
void CTRL_Init(void);
void CTRL_KeyStatusRefresh(void);

#endif
