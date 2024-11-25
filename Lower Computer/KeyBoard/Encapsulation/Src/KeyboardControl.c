#include "CTRL.h"
#include "KeyboardControl.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

/* 键盘映射表 */
uint8_t KeyboardCode[KEYMAPPING_MODE_NUM][KEYNUM + 1] =
{
	/* 常用模式 */
	{
		KEYBOARD_UPARROW, KEYBOARD_DOWNARROW, KEYBOARD_LEFTARROW, KEYBOARD_RIGHTARROW,
		KEYBOARD_d_AND_D, KEYBOARD_f_AND_F, KEYBOARD_j_AND_J, KEYBOARD_k_AND_K,
		KEYBOARD_z_AND_Z, KEYBOARD_x_AND_X, KEYBOARD_ESCAPE, KEYBOARD_ENTER
	},
	/* 5K模式 */
	{
		KEYBOARD_UPARROW, KEYBOARD_DOWNARROW, KEYBOARD_LEFTARROW, KEYBOARD_RIGHTARROW,
		KEYBOARD_d_AND_D, KEYBOARD_f_AND_F, KEYBOARD_j_AND_J, KEYBOARD_k_AND_K,
		KEYBOARD_ESCAPE, KEYBOARD_SPACEBAR, KEYBOARD_SPACEBAR, KEYBOARD_ENTER
	},
	/* 6K模式 */
	{
		KEYBOARD_UPARROW, KEYBOARD_DOWNARROW, KEYBOARD_LEFTARROW, KEYBOARD_RIGHTARROW,
		KEYBOARD_s_AND_S, KEYBOARD_d_AND_D, KEYBOARD_k_AND_K, KEYBOARD_l_AND_L,
		KEYBOARD_ESCAPE, KEYBOARD_f_AND_F, KEYBOARD_j_AND_J, KEYBOARD_ENTER
	},
	/* 7K模式 */
	{
		KEYBOARD_s_AND_S, KEYBOARD_d_AND_D, KEYBOARD_k_AND_K, KEYBOARD_l_AND_L,
		KEYBOARD_LEFTARROW, KEYBOARD_f_AND_F, KEYBOARD_j_AND_J, KEYBOARD_RIGHTARROW,
		KEYBOARD_ESCAPE, KEYBOARD_SPACEBAR, KEYBOARD_SPACEBAR, KEYBOARD_ENTER
	}
};

GPIO_PinState KeyboardStatus[KEYNUM];			//全键盘按键状态

uint8_t KeyboardNowStatus[SENDKEYNUM];			//当前按键状态

uint8_t USB_SendBuffer[SENDARRAYLENGTH] = {0};	//发送数组

/**
  * @brief  键盘控制
  * @param  无
  * @retval 无
  */
void KeyboardCTRL(void)
{
	uint8_t i, j;
	
	/* 键盘状态扫描 */
	/* 第一行 */
	KeyboardStatus[0] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	KeyboardStatus[1] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
	KeyboardStatus[2] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);
	KeyboardStatus[3] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3);
	/* 第二行 */
	KeyboardStatus[4] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);
	KeyboardStatus[5] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5);
	KeyboardStatus[6] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6);
	KeyboardStatus[7] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7);
	/* 第三行 */
	KeyboardStatus[8] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
	KeyboardStatus[9] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
	KeyboardStatus[10] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);
	KeyboardStatus[11] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11);
	
	/* 键盘优先级判断 */
	for(i = 0; i < SENDKEYNUM; i++)
	{
		KeyboardNowStatus[i] = 0;				//重置当前键盘状态数组
	}
	j = 0;
	for(i = 0; i < KEYNUM; i++)
	{
		if(KeyboardStatus[i] == GPIO_PIN_RESET)
		{
			KeyboardNowStatus[j++] = i + 1;		//自优先级选取按键,最多SENDKEYNUM个
			if(j >= SENDKEYNUM)
			{
				break;
			}
		}
	}
	
	/* 发送刷新后的键码 */
	for (i = 0; i < SENDKEYNUM; i++)
	{
		USB_SendBuffer[i + 2] = KeyboardCode[KeyMapping_Mode][KeyboardNowStatus[i] - 1];	//发送新按键状态
	}
	USBD_HID_SendReport(&hUsbDeviceFS, USB_SendBuffer, sizeof(USB_SendBuffer));
}
