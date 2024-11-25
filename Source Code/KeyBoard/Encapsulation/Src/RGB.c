#include <stdlib.h>

#include "rtc.h"
#include "tim.h"
#include "dma.h"
#include "RGB.h"
#include "CTRL.h"

RTC_DateTypeDef DataInfo;
RTC_TimeTypeDef TimeInfo;

RGB_Color_TypeDef Now_Color;		//当前颜色
RGB_Color_TypeDef Target_Color;		//目标颜色

/* 二维数组存放最终PWM输出数组,每一行24个数据代表一个LED,最后一行24个0代表RESET码 */
uint32_t Pixel_Buf[Pixel_NUM+1][24];

uint16_t LuminancelimitList[LUMINANCELIMIT_MODE_NUM] = {128, 112, 96, 80, 64, 48, 32};	//预设亮度极限值数组
uint16_t Luminancelimit;			//亮度极限值

uint16_t RGBChangeSpeedList[RGBCHANGE_MODE_NUM] = {48, 40, 32, 24, 16, 8};				//预设变化速度重装值数组
uint16_t RGBChangeSpeed;			//预设变化速度重装值
uint16_t RGBChangeSpeedCount;		//变化速度重装计数

uint8_t RGBFirstChangeFlag;			//初次变化标志

/**
  * @brief  RGB初始化
  * @param  无
  * @retval 无
  */
void RGB_Init(void)
{
	Luminancelimit = LuminancelimitList[0];					//初始亮度极限值
	RGBChangeSpeed = RGBChangeSpeedList[0];					//初始变化速度重装值
	RGBChangeSpeedCount = 0;								//变化速度重装计数归零
	RGBFirstChangeFlag = 1;									//初次变化标志置位
	
	HAL_RTC_GetTime(&hrtc, &TimeInfo, RTC_FORMAT_BIN);		//获取RTC时间
	HAL_RTC_GetDate(&hrtc, &DataInfo, RTC_FORMAT_BIN);		//获取RTC日期
	
	HAL_TIM_Base_Start(&htim3);								//启动TIM3定时器
	
	/* 设定随机数种子 */
	srand(TimeInfo.Hours + TimeInfo.Minutes + TimeInfo.Seconds + __HAL_TIM_GET_COUNTER(&htim3));
	
	/* 设定初始当前RGB值 */
	Now_Color.R = 0;
	Now_Color.G = 0;
	Now_Color.B = 0;
	
	/* 设定初始目标RGB值 */
	Target_Color.R = Luminancelimit - 1;
	Target_Color.G = Luminancelimit - 1;
	Target_Color.B = Luminancelimit - 1;
}

/**
  * @brief  设置单个RGB灯颜色
  * @param  无符号8位整型LED灯序号,RGB结构体颜色
  * @retval 无
  */
void RGB_SetColor(uint8_t LedId, RGB_Color_TypeDef Color)
{
	uint8_t i; 
	if(LedId > Pixel_NUM)
	{
		return;					//防止写入ID大于LED总数
	}
	
	for(i = 0; i < 8; i++)		//数组某一行0~7转化存放G
	{
		Pixel_Buf[LedId][i] = ((Color.G & (1 << (7 - i))) ? (CODE_1) : CODE_0);
	}
	for(i = 8; i < 16; i++)		//数组某一行8~15转化存放R
	{
		Pixel_Buf[LedId][i] = ((Color.R & (1 << (15 - i))) ? (CODE_1) : CODE_0);
	}
	for(i = 16; i < 24; i++)	//数组某一行16~23转化存放B
	{
		Pixel_Buf[LedId][i] = ((Color.B & (1 << (23 - i))) ? (CODE_1) : CODE_0);
	}
}

/**
  * @brief  末尾输出24个0作为重置延时
  * @param  无
  * @retval 无
  */
void Reset_Load(void)
{
	uint8_t i;
	for(i = 0; i < 24; i++)
	{
		Pixel_Buf[Pixel_NUM][i] = 0;
	}
}

/**
  * @brief  发送数组
  * @param  无
  * @retval 无
  */
void RGB_SendArray(void)
{
	MX_DMA_Init();
	MX_TIM1_Init();
	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t*)Pixel_Buf, (Pixel_NUM + 1) * 24);
}

/**
  * @brief  显示RGB颜色
  * @param  无符号16位整型LED灯数量,RGB结构体颜色
  * @retval 无
  */
void RGB_Show(uint16_t Pixel_Len, RGB_Color_TypeDef Color)
{
	uint16_t i;
	for(i = 0; i < Pixel_Len; i++)
	{
		RGB_SetColor(i, Color);
	}
	Reset_Load();
	RGB_SendArray();
}

/**
  * @brief  LED灯串颜色控制
  * @param  无符号16位整型LED灯数量
  * @retval 无
  */
void RGB_ColorCtrl(uint16_t Pixel_Len)
{
	if(RGBFirstChangeFlag)
	{
		/* 初次变化时无法修改菜单选项值 */
		KeyMapping_Mode = 0;
		Luminancelimit_Mode = 0;
		RBGChange_Mode = 0;
		
		RGBChangeSpeed = RGBChangeSpeedList[4];
	}
	else
	{
		Luminancelimit = LuminancelimitList[Luminancelimit_Mode];
		RGBChangeSpeed = RGBChangeSpeedList[RBGChange_Mode];
	}
	if(RGBChangeSpeedCount <= RGBChangeSpeed)
	{
		RGBChangeSpeedCount++;				//变化速度重装计数自增
		return;
	}
	RGBChangeSpeedCount = 0;
	
	/* 红色色调追踪 */
	if(Now_Color.R != Target_Color.R)		//当前颜色与目标颜色不同,每次循环使当前颜色逼近目标颜色
	{
		if(Now_Color.R > Target_Color.R)
		{
			Now_Color.R--;
		}
		else
		{
			Now_Color.R++;
		}
	}
	else                               		//当前颜色与目标颜色相同,随机产生新的与当前颜色不同的目标颜色
	{
		srand(TimeInfo.Hours + TimeInfo.Minutes + TimeInfo.Seconds + __HAL_TIM_GET_COUNTER(&htim3));
		Target_Color.R = rand() % Luminancelimit;
		while(Now_Color.R == Target_Color.R)
		{
			srand(TimeInfo.Hours + TimeInfo.Minutes + TimeInfo.Seconds + __HAL_TIM_GET_COUNTER(&htim3));
			Target_Color.R = rand() % Luminancelimit;
		}
	}
	
	/* 绿色色调追踪 */
	if(Now_Color.G != Target_Color.G)		//当前颜色与目标颜色不同,每次循环使当前颜色逼近目标颜色
	{
		if(Now_Color.G > Target_Color.G)
		{
			Now_Color.G--;
		}
		else
		{
			Now_Color.G++;
		}
	}
	else                                	//当前颜色与目标颜色相同,随机产生新的与当前颜色不同的目标颜色
	{
		srand(TimeInfo.Hours + TimeInfo.Minutes + TimeInfo.Seconds + __HAL_TIM_GET_COUNTER(&htim3));
		Target_Color.G = rand() % Luminancelimit;
		while(Now_Color.G == Target_Color.G)
		{
			srand(TimeInfo.Hours + TimeInfo.Minutes + TimeInfo.Seconds + __HAL_TIM_GET_COUNTER(&htim3));
			Target_Color.G = rand() % Luminancelimit;
		}
	}
	
	/* 蓝色色调追踪 */
	if(Now_Color.B != Target_Color.B)    	//当前颜色与目标颜色不同,每次循环使当前颜色逼近目标颜色
	{
		if(Now_Color.B > Target_Color.B)
		{
		Now_Color.B--;
		}
		else
		{
		Now_Color.B++;
		}
	}
	else                                	//当前颜色与目标颜色相同,随机产生新的与当前颜色不同的目标颜色
	{
		srand(TimeInfo.Hours + TimeInfo.Minutes + TimeInfo.Seconds + __HAL_TIM_GET_COUNTER(&htim3));
		Target_Color.B = rand() % Luminancelimit;
		while(Now_Color.B == Target_Color.B)
		{
			srand(TimeInfo.Hours + TimeInfo.Minutes + TimeInfo.Seconds + __HAL_TIM_GET_COUNTER(&htim3));
			Target_Color.B = rand() % Luminancelimit;
		}
	}
	RGB_Show(Pixel_Len, Now_Color);			//输出颜色
	
	/* 初次变化完成,复位初次变化标志 */
	if(RGBFirstChangeFlag 
		&& Target_Color.R != Luminancelimit - 1
		&& Target_Color.G != Luminancelimit - 1
		&& Target_Color.B != Luminancelimit - 1)
	{
		RGBFirstChangeFlag = 0;
	}
}
