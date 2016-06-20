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
  * ��̳    :http://www.firebbs.cn
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
#include "./Bsp/touch/gt9xx.h"
#include "GUI.h"
#include "DIALOG.h"
#include "GUIDEMO.h"

KEY Key1,Key2;



/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
//	static uint8_t count=0;
	/*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
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
	
	/* ��ʼ�������� */
	GTP_Init_Panel();	//��������ʼ������������ϵͳ�δ�ʱ����ʼ��
	
	/* ��ʼ��ϵͳ�δ�ʱ�� */
	SysTick_Init();
	
	TPAD_Init();
	
  /*��ʼ��SDRAMģ��*/
  SDRAM_Init();

	/* Init the STemWin GUI Library */
  GUI_Init();
	
  /* Setup layer configuration during startup */
//  GUI_SetBkColor(GUI_TRANSPARENT);
//  GUI_SelectLayer(1);
//  GUI_Clear();
//  GUI_SetBkColor(GUI_TRANSPARENT); 
//  GUI_SelectLayer(0);

	while(1)
	{
		GUIDEMO_Main();
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
