#ifndef __RGB_H__
#define __RGB_H__

#include "main.h"

#define CODE_1       				(58)	//1码定时器计数次数
#define CODE_0       				(25)	//0码定时器计数次数

#define Pixel_NUM 					13		//定义RGB数量为真实数量 + 1,以保证末尾RGB显示正常

/* 数组下标值定义 */
#define LUMINANCELIMIT_MODE_NUM		7
#define RGBCHANGE_MODE_NUM			6

/* 定义单个LED三原色值大小的结构体 */
typedef struct
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
}RGB_Color_TypeDef;

extern uint16_t Luminancelimit;		//外部调用声明

/* 外部可调用函数 */
void RGB_Init(void);
void RGB_ColorCtrl(uint16_t Pixel_Len);

/* 内部函数 */
//void RGB_SetColor(uint8_t LedId, RGB_Color_TypeDef Color);
//void Reset_Load(void);
//void RGB_SendArray(void);
//void RGB_Show(uint16_t Pixel_Len, RGB_Color_TypeDef Color);

#endif
