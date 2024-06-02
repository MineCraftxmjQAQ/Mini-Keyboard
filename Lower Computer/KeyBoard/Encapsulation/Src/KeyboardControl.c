#include "stm32f1xx_hal.h"
#include "usbd_hid.h"
#include "KeyboardControl.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

uint8_t KeyboardCode[KEYNUM + 1] = {0, KEYBOARD_A, KEYBOARD_B, KEYBOARD_C, 
									   KEYBOARD_D, KEYBOARD_E, KEYBOARD_F,
									   KEYBOARD_G, KEYBOARD_H, KEYBOARD_I, 
									   KEYBOARD_J, KEYBOARD_K, KEYBOARD_L};

GPIO_PinState KeyboardStatus[KEYNUM];	//全键盘按键状态
									   
uint8_t KeyboardNowStatus[SENDKEYNUM];	//当前按键状态
uint8_t KeyboardLastStatus[SENDKEYNUM];	//上一次按键状态
									   
uint8_t USB_SendBuffer[SENDARRAYLENGTH] = {0};	//发送数组

/**
  * @brief  键位配置
  * @param  无
  * @retval 无
  */
void KeyboardInit(void)
{

}

/**
  * @brief  键盘状态刷新
  * @param  无
  * @retval 无
  */
void KeyboardStatusRefresh(void)
{
	KeyboardStatus[0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	KeyboardStatus[1] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
	KeyboardStatus[2] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);
	KeyboardStatus[3] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3);
	
	KeyboardStatus[4] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);
	KeyboardStatus[5] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);
	KeyboardStatus[6] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);
	KeyboardStatus[7] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);
	
	KeyboardStatus[8] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	KeyboardStatus[9] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
	KeyboardStatus[10] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);
	KeyboardStatus[11] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11);
}

/**
  * @brief  键盘优先级控制
  * @param  无
  * @retval 无
  */
void KeyboardPriorityControl(void)
{
	uint8_t i, j;
	for (i = 0; i < SENDKEYNUM; i++)
	{
		KeyboardNowStatus[i] = 0;	//重置当前键盘状态数组
	}
	j = 0;
	for (i = 0; i < KEYNUM; i++)
	{
		if (KeyboardStatus[i] == GPIO_PIN_RESET)
		{
			KeyboardNowStatus[j++] = i + 1;	//自优先级选取按键,最多SENDKEYNUM个
			if (j >= SENDKEYNUM) break;
		}
	}
}

/**
  * @brief  键盘发送
  * @param  无
  * @retval 无
  */
void KeyboardSendCode(void)
{
	uint8_t i;
	KeyboardStatusRefresh();
	KeyboardPriorityControl();
	for (i = 0; i < SENDKEYNUM; i++)
	{
		if (KeyboardLastStatus[i] != KeyboardNowStatus[i]) break;
	}
	if (i < SENDKEYNUM)
	{
		for (i = 0; i < SENDKEYNUM; i++)
		{
			USB_SendBuffer[i + 2] = KeyboardCode[KeyboardNowStatus[i]];	//发送新按键状态
			KeyboardLastStatus[i] = KeyboardNowStatus[i];//转移新按键状态
		}
		USBD_HID_SendReport(&hUsbDeviceFS, USB_SendBuffer, sizeof(USB_SendBuffer));
	}
}
