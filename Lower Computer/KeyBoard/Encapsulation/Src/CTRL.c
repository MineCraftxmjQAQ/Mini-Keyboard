#include "CTRL.h"
#include "RGB.h"
#include "OLED.h"
#include "KeyboardControl.h"

uint8_t KeyMapping_Mode;		//键位映射控制菜单值
uint8_t Luminancelimit_Mode;	//RGB亮度极限菜单值
uint8_t RBGChange_Mode;			//RBG闪烁模式菜单值

GPIO_PinState CTRLKeyStatus[3];	//实际按键按下状态数组
uint8_t ButtonStatus[3];		//按键状态机切换数组

/**
  * @brief  控制按键初始化
  * @param  无
  * @retval 无
  */
void CTRL_Init(void)
{
	/* 初始时三个菜单均选择第0号项 */
	KeyMapping_Mode = 0;
	Luminancelimit_Mode = 0;
	RBGChange_Mode = 0;
	
	/* 初始时按键均为未触发状态 */
	ButtonStatus[0] = 0;
	ButtonStatus[1] = 0;
	ButtonStatus[2] = 0;
	
	/* OLED显示初始化 */
	OLED_Init();
	OLED_Clear();
	
	/* OLED初始内容显示 */
	OLED_ShowString(1, 1, "   CTRL MENU");
	OLED_ShowString(2, 1, "1:Mapping->[ 0 ]");
	OLED_ShowString(3, 1, "2:Lmlimit->[ 0 ]");
	OLED_ShowString(4, 1, "3:RBGMode->[ 0 ]");
}

/**
  * @brief  控制按键状态刷新
  * @param  无
  * @retval 无
  */
void CTRL_KeyStatusRefresh(void)
{
	/* 按键状态刷新 */
	CTRLKeyStatus[0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
	CTRLKeyStatus[1] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);
	CTRLKeyStatus[2] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3);
	
	/* 当按键按下且按键状态机处于未触发状态则置按键状态机为触发状态 */
	if(CTRLKeyStatus[0] == GPIO_PIN_RESET && ButtonStatus[0] == 0)
	{
		ButtonStatus[0] = 1;
	}
	if(CTRLKeyStatus[1] == GPIO_PIN_RESET && ButtonStatus[1] == 0)
	{
		ButtonStatus[1] = 1;
	}
	if(CTRLKeyStatus[2] == GPIO_PIN_RESET && ButtonStatus[2] == 0)
	{
		ButtonStatus[2] = 1;
	}
	
	/* 当按键松开时则更改按键状态机为未触发状态 */
	if(CTRLKeyStatus[0] == GPIO_PIN_SET)
	{
		ButtonStatus[0] = 0;
	}
	if(CTRLKeyStatus[1] == GPIO_PIN_SET)
	{
		ButtonStatus[1] = 0;
	}
	if(CTRLKeyStatus[2] == GPIO_PIN_SET)
	{
		ButtonStatus[2] = 0;
	}

	/* 当按键状态机为触发状态时修改菜单选项并置按键状态机为未触发状态 */
	if(ButtonStatus[0] == 1)
	{
		ButtonStatus[0] = 2;
		KeyMapping_Mode++;
		if(KeyMapping_Mode > KEYMAPPING_MODE_NUM - 1)
		{
			KeyMapping_Mode = 0;
		}
	}
	if(ButtonStatus[1] == 1)
	{
		ButtonStatus[1] = 2;
		Luminancelimit_Mode++;
		if(Luminancelimit_Mode > LUMINANCELIMIT_MODE_NUM - 1)
		{
			Luminancelimit_Mode = 0;
		}
	}
	if(ButtonStatus[2] == 1)
	{
		ButtonStatus[2] = 2;
		RBGChange_Mode++;
		if(RBGChange_Mode > RGBCHANGE_MODE_NUM - 1)
		{
			RBGChange_Mode = 0;
		}
	}
	
	/* 更新OLED显示的菜单选项 */
	OLED_ShowNum(2, 14, KeyMapping_Mode, 1);
	OLED_ShowNum(3, 14, Luminancelimit_Mode, 1);
	OLED_ShowNum(4, 14, RBGChange_Mode, 1);
}
