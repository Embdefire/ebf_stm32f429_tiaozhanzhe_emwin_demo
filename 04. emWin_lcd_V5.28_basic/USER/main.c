/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ��1.5.1�汾�⽨�Ĺ���ģ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F429 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "stm32f4xx.h"
#include "./Bsp/led/bsp_led.h" 
#include "./Bsp/beep/bsp_beep.h" 
#include "./Bsp/usart/bsp_debug_usart.h"
#include "./Bsp/systick/bsp_SysTick.h"
#include "./Bsp/TouchPad/bsp_touchpad.h"
#include "./Bsp/key/bsp_key.h"
#include "./Bsp/sdram/bsp_sdram.h"
#include "./Bsp/lcd/bsp_lcd.h"

#include "GUI.h"
#include "DIALOG.h"


KEY Key1,Key2;
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	static uint8_t count=0;
	/*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
	
	/* ��ʼ��LED */
	LED_GPIO_Config();
	/* ��ʼ�������� */
	Beep_GPIO_Config();
	Key1_GPIO_Config();
	Key2_GPIO_Config();
	KeyCreate(&Key1,GetPinStateOfKey1);
	KeyCreate(&Key2,GetPinStateOfKey2);
	/* ��ʼ�����Դ��ڣ�һ��Ϊ����1 */
	Debug_USART_Config();
	/* ��ʼ��ϵͳ�δ�ʱ�� */
	SysTick_Init();
	printf("TouchPAD Demo\n");
	
	TPAD_Init();
	
  /*��ʼ��SDRAMģ��*/
  SDRAM_Init();

	/* Init the STemWin GUI Library */
  GUI_Init();
	
  /* Setup layer configuration during startup */
  GUI_SetBkColor(GUI_TRANSPARENT);
  GUI_SelectLayer(1);
  GUI_Clear();
  GUI_SetBkColor(GUI_TRANSPARENT); 
  GUI_SelectLayer(0);
	
	
	GUI_SelectLayer(1);
	GUI_SetColor(GUI_YELLOW);
	GUI_FillRect(50,300, 750, 450);
	GUI_FillRect(150,100, 250, 200);	
	
	GUI_SelectLayer(0);
	GUI_SetColor(GUI_RED);
	GUI_FillRect(50,60, 750, 250);
	GUI_SetFont(GUI_FONT_32B_ASCII);
	GUI_DispStringAt("Hello World",10,10);
	GUI_SetFont(GUI_FONT_13B_ASCII);
	GUI_DispStringAt("Hello World",200,10);
	
	while(1)
	{
		Key_RefreshState(&Key1);//ˢ�°���״̬
		Key_RefreshState(&Key2);//ˢ�°���״̬
		if(Key_AccessTimes(&Key1,KEY_ACCESS_READ)!=0)//���������¹�
		{
			LED2_TOGGLE;
			Key_AccessTimes(&Key1,KEY_ACCESS_WRITE_CLEAR);
		}
		if(Key_AccessTimes(&Key2,KEY_ACCESS_READ)!=0)//���������¹�
		{
			LED1_TOGGLE;
			Key_AccessTimes(&Key2,KEY_ACCESS_WRITE_CLEAR);
		}
		if(TPAD_Scan(0))
		{
			BEEP_ON;
			GUI_Delay(100);
		}
		if(count==100)
		{
			LED3_ON;
		}else if(count==110)
		{
			count=0;
			LED3_OFF;
		}
		count++;
		BEEP_OFF;			
		GUI_Delay(10);
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
